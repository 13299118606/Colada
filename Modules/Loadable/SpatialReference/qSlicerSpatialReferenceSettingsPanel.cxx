/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Johan Andruejol, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// CTK Includes
#include "ctkCollapsibleGroupBox.h"
#include "ctkSettingsPanel.h"

// Qt includes
#include <QTableView>
#include <QDebug>
#include <QLabel>
#include <QHash>
#include <QSettings>
#include <QLineEdit>

// QtGUI includes
#include "qMRMLNodeComboBox.h"
#include "qMRMLUnitWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerSpatialReferenceSettingsPanel.h"
#include "ui_qSlicerSpatialReferenceSettingsPanel.h"

// Colada includes
#include "qCRSDropLineEdit.h"
#include "util.h"

// h5geo includes
#define H5GEO_USE_GDAL
#include <h5geo/h5core.h>

// GDAL includes
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

// --------------------------------------------------------------------------
// qSlicerSpatialReferenceSettingsPanelPrivate

//-----------------------------------------------------------------------------
class qSlicerSpatialReferenceSettingsPanelPrivate: public Ui_qSlicerSpatialReferenceSettingsPanel
{
  Q_DECLARE_PUBLIC(qSlicerSpatialReferenceSettingsPanel);
protected:
  qSlicerSpatialReferenceSettingsPanel* const q_ptr;

public:
  qSlicerSpatialReferenceSettingsPanelPrivate(qSlicerSpatialReferenceSettingsPanel& object);
  void init();
};

// --------------------------------------------------------------------------
// qSlicerSpatialReferenceSettingsPanelPrivate methods

// --------------------------------------------------------------------------
qSlicerSpatialReferenceSettingsPanelPrivate
::qSlicerSpatialReferenceSettingsPanelPrivate(qSlicerSpatialReferenceSettingsPanel& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerSpatialReferenceSettingsPanelPrivate::init()
{
  Q_Q(qSlicerSpatialReferenceSettingsPanel);

  this->setupUi(q);

  this->CRSDropLineEdit->setCRSName(util::CRSName());

  q->registerProperty(
        "SpatialReference/CRSName", this->CRSDropLineEdit, "CRSName", SIGNAL(CRSNameChanged(QString)));
  q->registerProperty(
        "SpatialReference/CRSAuthName", this->CRSDropLineEdit, "CRSAuthName", SIGNAL(CRSAuthNameChanged(QString)));
  q->registerProperty(
        "SpatialReference/CRSCode", this->CRSDropLineEdit, "CRSCode", SIGNAL(CRSCodeChanged(QString)));

  // create connection with `Units`
  qSlicerApplication* app = qSlicerApplication::application();
  if (!app){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: unable to get app instance";
    return;
  }

  ctkSettingsPanel* unitsPanel = app->settingsDialog()->panel("Units");

  if (!unitsPanel){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: " <<
                   "unable to get `Units` panel";
    return;
  }

  QList<ctkCollapsibleGroupBox*> groupBoxList =
      unitsPanel->findChildren<ctkCollapsibleGroupBox*>();

  ctkCollapsibleGroupBox* lengthGroupBox = nullptr;
  ctkCollapsibleGroupBox* timeGroupBox = nullptr;
  for (int i = 0; i < groupBoxList.count(); i++){
    if (groupBoxList[i]->title() == "Length"){
      lengthGroupBox = groupBoxList[i];
    } else if (groupBoxList[i]->title() == "Time"){
      timeGroupBox = groupBoxList[i];
    }
  }

  if (!lengthGroupBox){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: " <<
                   "`Length` unit group box widget not found (needed for SpatialReference settings)";
    return;
  }

  if (!timeGroupBox){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: " <<
                   "`Time` unit group box widget not found (needed for SpatialReference settings)";
    return;
  }

  qMRMLUnitWidget* lengthUnitWidget = lengthGroupBox->findChild<qMRMLUnitWidget*>();

  if (!lengthUnitWidget){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: " <<
                   "`Length` unit widget not found (needed for SpatialReference settings)";
    return;
  }

  qMRMLUnitWidget* timeUnitWidget = timeGroupBox->findChild<qMRMLUnitWidget*>();

  if (!timeUnitWidget){
    qCritical() << "qSlicerSpatialReferenceSettingsPanelPrivate::init: " <<
                   "`Time` unit widget not found (needed for SpatialReference settings)";
    return;
  }

  q->onLengthUnitWidgetSuffixChanged(lengthUnitWidget->suffix());
  q->connect(lengthUnitWidget, &qMRMLUnitWidget::suffixChanged,
             q, &qSlicerSpatialReferenceSettingsPanel::onLengthUnitWidgetSuffixChanged, Qt::QueuedConnection);
  q->onTimeUnitWidgetSuffixChanged(timeUnitWidget->suffix());
  q->connect(timeUnitWidget, &qMRMLUnitWidget::suffixChanged,
             q, &qSlicerSpatialReferenceSettingsPanel::onTimeUnitWidgetSuffixChanged, Qt::QueuedConnection);
  q->onCRSChanged(CRSDropLineEdit->CRSName(), CRSDropLineEdit->CRSAuthName(), CRSDropLineEdit->CRSCode());
  q->connect(CRSDropLineEdit, &qCRSDropLineEdit::CRSChanged,
             q, &qSlicerSpatialReferenceSettingsPanel::onCRSChanged, Qt::QueuedConnection);
}

// --------------------------------------------------------------------------
// qSlicerSpatialReferenceSettingsPanel methods

// --------------------------------------------------------------------------
qSlicerSpatialReferenceSettingsPanel::qSlicerSpatialReferenceSettingsPanel(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerSpatialReferenceSettingsPanelPrivate(*this))
{
  Q_D(qSlicerSpatialReferenceSettingsPanel);
  d->init();

  d->CRSWidget->setObjectName("CRSWidget");
  d->CRSWidget->getTableView()->setDragEnabled(true);
  d->CRSWidget->getUnitsLineEdit()->setReadOnly(true);
}

// --------------------------------------------------------------------------
qSlicerSpatialReferenceSettingsPanel::~qSlicerSpatialReferenceSettingsPanel() = default;

void qSlicerSpatialReferenceSettingsPanel::onTimeUnitWidgetSuffixChanged(QString name){
  if (name.isEmpty())
    return;

  h5geo::sr::setTemporalUnits(name.toStdString());
}

void qSlicerSpatialReferenceSettingsPanel::onLengthUnitWidgetSuffixChanged(QString name){
  Q_D(qSlicerSpatialReferenceSettingsPanel);

  if (name.isEmpty())
    return;

  d->CRSWidget->getUnitsLineEdit()->setText(name);

  // update CRS and units if it is not empty
  onCRSChanged(d->CRSDropLineEdit->CRSName(),
               d->CRSDropLineEdit->CRSAuthName(),
               d->CRSDropLineEdit->CRSCode());
}

void qSlicerSpatialReferenceSettingsPanel::onCRSChanged(
    const QString& name,
    const QString& authName,
    const QString& code)
{
  Q_D(qSlicerSpatialReferenceSettingsPanel);

  if (authName.isEmpty() || code.isEmpty())
    return;

  // Length units must be set after reference.
  // Also setting length units on invalid reference has not effect.
  // If units are changed then returning Authority name/code is useless
  // as spatial reference changed then

  h5geo::sr::setSpatialReferenceFromUserInput(
        authName.trimmed().toStdString(),
        code.trimmed().toStdString());

  QString lengthUnits = d->CRSWidget->getUnitsLineEdit()->text().trimmed();
  if (!lengthUnits.isEmpty())
    h5geo::sr::setLengthUnits(lengthUnits.toStdString());
}
