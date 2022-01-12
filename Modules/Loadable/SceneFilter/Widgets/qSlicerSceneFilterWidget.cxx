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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Stack3D Widgets includes
#include "qSlicerSceneFilterWidget.h"
#include "ui_qSlicerSceneFilterWidget.h"
#include "vtkSlicerSceneFilterLogic.h"

// Qt includes
#include <QToolButton>

// Slicer includes
#include "qSlicerApplication.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLDisplayableNode.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Axes
class qSlicerSceneFilterWidgetPrivate
  : public Ui_qSlicerSceneFilterWidget
{
  Q_DECLARE_PUBLIC(qSlicerSceneFilterWidget);
protected:
  qSlicerSceneFilterWidget* const q_ptr;

public:
  qSlicerSceneFilterWidgetPrivate(
    qSlicerSceneFilterWidget& object);
  virtual void setupUi(qSlicerSceneFilterWidget*);

  qSlicerApplication* app = nullptr;
  vtkSlicerSceneFilterLogic* logic = nullptr;
};

// --------------------------------------------------------------------------
qSlicerSceneFilterWidgetPrivate
::qSlicerSceneFilterWidgetPrivate(
  qSlicerSceneFilterWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerSceneFilterWidgetPrivate
::setupUi(qSlicerSceneFilterWidget* widget)
{
  Q_Q(qSlicerSceneFilterWidget);
  this->Ui_qSlicerSceneFilterWidget::setupUi(widget);

  app = qSlicerApplication::application();
  if (app)
    logic = vtkSlicerSceneFilterLogic::SafeDownCast(
          app->moduleLogic("SceneFilter"));
}

//-----------------------------------------------------------------------------
// qSlicerSceneFilterWidget methods

//-----------------------------------------------------------------------------
qSlicerSceneFilterWidget
::qSlicerSceneFilterWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerSceneFilterWidgetPrivate(*this) )
{
  Q_D(qSlicerSceneFilterWidget);
  d->setupUi(this);

  this->onDomainComboBoxCurrentTextChanged(d->SceneFilterDomainComboBox->currentText());

  connect(d->SceneFilterDomainComboBox, &QComboBox::currentTextChanged,
          this, &qSlicerSceneFilterWidget::onDomainComboBoxCurrentTextChanged);

  connect(d->SceneFilterAttributeLineEdit, &QLineEdit::editingFinished,
          this, &qSlicerSceneFilterWidget::onLineEditEditingFinished);
  connect(d->SceneFilterValueLineEdit, &QLineEdit::editingFinished,
          this, &qSlicerSceneFilterWidget::onLineEditEditingFinished);
}

//-----------------------------------------------------------------------------
qSlicerSceneFilterWidget
::~qSlicerSceneFilterWidget()
{
}

void qSlicerSceneFilterWidget::onLineEditEditingFinished()
{
  Q_D(qSlicerSceneFilterWidget);
  d->logic->setAttributeFilter(
        d->SceneFilterAttributeLineEdit->text().toStdString(),
        d->SceneFilterValueLineEdit->text().toStdString());
}

void qSlicerSceneFilterWidget::onDomainComboBoxCurrentTextChanged(const QString& text)
{
  Q_D(qSlicerSceneFilterWidget);
  d->logic->setDomainFilter(text.toStdString());
}
