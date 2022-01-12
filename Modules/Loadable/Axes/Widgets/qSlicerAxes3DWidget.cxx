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
#include "qSlicerAxes3DWidget.h"
#include "ui_qSlicerAxes3DWidget.h"
#include "vtkSlicerAxesLogic.h"

// Qt includes
#include <QToolButton>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// CTK includes
#include "ctkPopupWidget.h"
#include "ctkDoubleSlider.h"
#include "ctkSliderWidget.h"

// VTK includes
#include "vtkCubeAxesActor.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLAxesDisplayableManager3D.h"
#include "vtkMRMLDisplayableNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDViewControllerWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Axes
class qSlicerAxes3DWidgetPrivate
  : public Ui_qSlicerAxes3DWidget
{
  Q_DECLARE_PUBLIC(qSlicerAxes3DWidget);
protected:
  qSlicerAxes3DWidget* const q_ptr;

public:
  qSlicerAxes3DWidgetPrivate(
    qSlicerAxes3DWidget& object);
  virtual void setupUi(qSlicerAxes3DWidget*);

  vtkMRMLAxesDisplayableManager3D*
  getCurrentAxesDisplayableManager3D();

  qSlicerApplication* app = nullptr;
  vtkSlicerAxesLogic* logic = nullptr;
};

// --------------------------------------------------------------------------
qSlicerAxes3DWidgetPrivate
::qSlicerAxes3DWidgetPrivate(
  qSlicerAxes3DWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerAxes3DWidgetPrivate
::setupUi(qSlicerAxes3DWidget* widget)
{
  Q_Q(qSlicerAxes3DWidget);

  this->Ui_qSlicerAxes3DWidget::setupUi(widget);

  app = qSlicerApplication::application();
  if (app)
    logic = vtkSlicerAxesLogic::SafeDownCast(
          app->moduleLogic("Axes"));
}

vtkMRMLAxesDisplayableManager3D*
qSlicerAxes3DWidgetPrivate::getCurrentAxesDisplayableManager3D()
{
  if (!viewNodeSelector->currentNode())
    return nullptr;

  char* name = viewNodeSelector->currentNode()->GetName();
  qMRMLThreeDWidget* threeDWidget = app->layoutManager()->threeDWidget(
        QString::fromUtf8(name));

  if (!threeDWidget)
    return nullptr;

  return vtkMRMLAxesDisplayableManager3D::SafeDownCast(
        threeDWidget->threeDView()->
        displayableManagerByClassName(
          "vtkMRMLAxesDisplayableManager3D"));
}

//-----------------------------------------------------------------------------
// qSlicerAxes3DWidget methods

//-----------------------------------------------------------------------------
qSlicerAxes3DWidget
::qSlicerAxes3DWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerAxes3DWidgetPrivate(*this) )
{
  Q_D(qSlicerAxes3DWidget);
  d->setupUi(this);

  connect(d->viewNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onCurrentViewNodeChanged(vtkMRMLNode*)));
  connect(d->nodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onCurrentNodeChanged(vtkMRMLNode*)));
  connect(d->showAxesBtn, &QAbstractButton::toggled,
          this, &qSlicerAxes3DWidget::onAxesBtnToggled);

  QList<ctkCollapsibleGroupBox*> groupBoxList(
        {
          d->generalSettingsCollapsibleGroupBox,
          d->axesSettingsCollapsibleGroupBox,
          d->axesLabelsSettingsCollapsibleGroupBox,
          d->axesTitlesCollapsibleGroupBox,
          d->gridCollapsibleGroupBox,
          d->innerGridCollapsibleGroupBox,
          d->gridpolysCollapsibleGroupBox
        });

  for (ctkCollapsibleGroupBox* groupBox : groupBoxList){
    for (const auto& child : groupBox->children()){
      QCheckBox* checkBox = qobject_cast<QCheckBox*>(child);
      if (checkBox){
        connect(checkBox, &QCheckBox::stateChanged,
                this, &qSlicerAxes3DWidget::onCheckBoxStateChanged);
        continue;
      }
      ctkColorPickerButton* colorBtn = qobject_cast<ctkColorPickerButton*>(child);
      if (colorBtn){
        connect(colorBtn, &ctkColorPickerButton::colorChanged,
                this, &qSlicerAxes3DWidget::onColorChanged);
        continue;
      }
      QLineEdit* lineEdit = qobject_cast<QLineEdit*>(child);
      if (lineEdit){
        connect(lineEdit, &QLineEdit::textChanged,
                this, &qSlicerAxes3DWidget::onTextChanged);
        continue;
      }
      QSpinBox* spinBox = qobject_cast<QSpinBox*>(child);
      if (spinBox){
        connect(spinBox, qOverload<int>(&QSpinBox::valueChanged),
                this, &qSlicerAxes3DWidget::onSpinBoxValueChanged);
        continue;
      }
      QDoubleSpinBox* dSpinBox = qobject_cast<QDoubleSpinBox*>(child);
      if (dSpinBox){
        connect(dSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
                this, &qSlicerAxes3DWidget::onDoubleSpinBoxValueChanged);
        continue;
      }
      QComboBox* combo = qobject_cast<QComboBox*>(child);
      if (combo){
        connect(combo, qOverload<int>(&QComboBox::currentIndexChanged),
                this, &qSlicerAxes3DWidget::onComboBoxIndexChanged);
        continue;
      }
      ctkSliderWidget* sliderWidget = qobject_cast<ctkSliderWidget*>(child);
      if (sliderWidget){
        connect(sliderWidget, &ctkSliderWidget::valueChanged,
                this, &qSlicerAxes3DWidget::onDoubleSpinBoxValueChanged);
        continue;
      }
    }
  }
}

//-----------------------------------------------------------------------------
qSlicerAxes3DWidget
::~qSlicerAxes3DWidget()
{
}

void qSlicerAxes3DWidget::onCurrentNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerAxes3DWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onCurrentViewNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerAxes3DWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  if (!qvtkIsConnected(axesActor, vtkCommand::ModifiedEvent,
                       this, SLOT(onCubeAxesActorModified(vtkObject*)))){
    qvtkConnect(axesActor, vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetXAxesLinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetYAxesLinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetZAxesLinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetLabelTextProperty(0), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetLabelTextProperty(1), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetLabelTextProperty(2), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetTitleTextProperty(0), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetTitleTextProperty(1), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetTitleTextProperty(2), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetXAxesGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetYAxesGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetZAxesGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetXAxesInnerGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetYAxesInnerGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetZAxesInnerGridlinesProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));

    qvtkConnect(axesActor->GetXAxesGridpolysProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetYAxesGridpolysProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
    qvtkConnect(axesActor->GetZAxesGridpolysProperty(), vtkCommand::ModifiedEvent,
                this, SLOT(onCubeAxesActorModified(vtkObject*)));
  }

  setupWidgetFromAxes();
}

void qSlicerAxes3DWidget::onCheckBoxStateChanged(int state)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onColorChanged(QColor c)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onTextChanged(const QString& text)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onSpinBoxValueChanged(int val)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onDoubleSpinBoxValueChanged(double val)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onComboBoxIndexChanged(int val)
{
  setupAxesFromWidget();
}

void qSlicerAxes3DWidget::onAxesBtnToggled(bool val)
{
  Q_D(qSlicerAxes3DWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D =
      d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  setupAxesFromWidget();
  axesActor->SetVisibility(val);

  emit axes3DVisibilityToggled(val, d->viewNodeSelector->currentNode());
}

void qSlicerAxes3DWidget::onCubeAxesActorModified(vtkObject* sender)
{
  setupWidgetFromAxes();
}

void qSlicerAxes3DWidget::setupWidgetFromAxes()
{
  Q_D(qSlicerAxes3DWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D =
      d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  vtkMRMLDisplayableNode* dispNode =
      vtkMRMLDisplayableNode::SafeDownCast(
        d->nodeSelector->currentNode());

  if (dispNode)
    axesManager3D->SetAndObserveMRMLNodeBounds(dispNode);

  // General Settings
  d->xAxisCheckBox->setChecked(
          axesActor->GetXAxisVisibility());
  d->yAxisCheckBox->setChecked(
          axesActor->GetYAxisVisibility());
  d->zAxisCheckBox->setChecked(
          axesActor->GetZAxisVisibility());

  d->xTicksCheckBox->setChecked(
          axesActor->GetXAxisTickVisibility());
  d->yTicksCheckBox->setChecked(
          axesActor->GetYAxisTickVisibility());
  d->zTicksCheckBox->setChecked(
          axesActor->GetZAxisTickVisibility());

  d->xMinorTicksCheckBox->setChecked(
          axesActor->GetXAxisMinorTickVisibility());
  d->yMinorTicksCheckBox->setChecked(
          axesActor->GetYAxisMinorTickVisibility());
  d->zMinorTicksCheckBox->setChecked(
          axesActor->GetZAxisMinorTickVisibility());

  d->xLabelCheckBox->setChecked(
          axesActor->GetXAxisLabelVisibility());
  d->yLabelCheckBox->setChecked(
          axesActor->GetYAxisLabelVisibility());
  d->zLabelCheckBox->setChecked(
          axesActor->GetZAxisLabelVisibility());

  d->xGridCheckBox->setChecked(
          axesActor->GetDrawXGridlines());
  d->yGridCheckBox->setChecked(
          axesActor->GetDrawYGridlines());
  d->zGridCheckBox->setChecked(
          axesActor->GetDrawZGridlines());

  d->xInnerGridCheckBox->setChecked(
          axesActor->GetDrawXInnerGridlines());
  d->yInnerGridCheckBox->setChecked(
          axesActor->GetDrawYInnerGridlines());
  d->zInnerGridCheckBox->setChecked(
          axesActor->GetDrawZInnerGridlines());

  d->xGridpolysCheckBox->setChecked(
          axesActor->GetDrawXGridpolys());
  d->yGridpolysCheckBox->setChecked(
          axesActor->GetDrawYGridpolys());
  d->zGridpolysCheckBox->setChecked(
          axesActor->GetDrawZGridpolys());

  d->flyModeCombo->setCurrentIndex(
        axesActor->GetFlyMode());
  d->gridLocationCombo->setCurrentIndex(
        axesActor->GetGridLineLocation());
  d->tickLocationCombo->setCurrentIndex(
        axesActor->GetTickLocation());

  // Axes settings
  d->xAxisWidthSpinBox->setValue(axesActor->GetXAxesLinesProperty()->GetLineWidth());
  d->yAxisWidthSpinBox->setValue(axesActor->GetYAxesLinesProperty()->GetLineWidth());
  d->zAxisWidthSpinBox->setValue(axesActor->GetZAxesLinesProperty()->GetLineWidth());

  d->xAxisTubeCheckBox->setChecked(
          axesActor->GetXAxesLinesProperty()->GetRenderLinesAsTubes());
  d->yAxisTubeCheckBox->setChecked(
          axesActor->GetYAxesLinesProperty()->GetRenderLinesAsTubes());
  d->zAxisTubeCheckBox->setChecked(
          axesActor->GetZAxesLinesProperty()->GetRenderLinesAsTubes());

  double c[3];
  axesActor->GetXAxesLinesProperty()->GetColor(c);
  d->xAxisColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetYAxesLinesProperty()->GetColor(c);
  d->yAxisColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetZAxesLinesProperty()->GetColor(c);
  d->zAxisColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  // Labels (numbers) settings
  axesActor->GetLabelTextProperty(0)->GetColor(c);
  d->xAxisLabelColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetLabelTextProperty(1)->GetColor(c);
  d->yAxisLabelColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetLabelTextProperty(2)->GetColor(c);
  d->zAxisLabelColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  d->xAxisLabelFontSizeSpinBox->setValue(axesActor->GetLabelTextProperty(0)->GetFontSize());
  d->yAxisLabelFontSizeSpinBox->setValue(axesActor->GetLabelTextProperty(1)->GetFontSize());
  d->zAxisLabelFontSizeSpinBox->setValue(axesActor->GetLabelTextProperty(2)->GetFontSize());

  d->xAxisLabelOffsetSpinBox->setValue(axesActor->GetLabelTextProperty(0)->GetLineOffset());
  d->yAxisLabelOffsetSpinBox->setValue(axesActor->GetLabelTextProperty(1)->GetLineOffset());
  d->zAxisLabelOffsetSpinBox->setValue(axesActor->GetLabelTextProperty(2)->GetLineOffset());

  d->xAxisLabelBoldCheckBox->setChecked(axesActor->GetLabelTextProperty(0)->GetBold());
  d->xAxisLabelBoldCheckBox->setChecked(axesActor->GetLabelTextProperty(1)->GetBold());
  d->zAxisLabelBoldCheckBox->setChecked(axesActor->GetLabelTextProperty(2)->GetBold());

  d->xAxisLabelItalicCheckBox->setChecked(axesActor->GetLabelTextProperty(0)->GetItalic());
  d->yAxisLabelItalicCheckBox->setChecked(axesActor->GetLabelTextProperty(0)->GetItalic());
  d->zAxisLabelItalicCheckBox->setChecked(axesActor->GetLabelTextProperty(0)->GetItalic());

  // Titles settings
  d->xAxisTitleLineEdit->setText(axesActor->GetXTitle());
  d->yAxisTitleLineEdit->setText(axesActor->GetYTitle());
  d->zAxisTitleLineEdit->setText(axesActor->GetZTitle());

  d->xAxisTitleOffsetSpinBox->setValue(axesActor->GetTitleTextProperty(0)->GetLineOffset());
  d->yAxisTitleOffsetSpinBox->setValue(axesActor->GetTitleTextProperty(1)->GetLineOffset());
  d->zAxisTitleOffsetSpinBox->setValue(axesActor->GetTitleTextProperty(2)->GetLineOffset());

  axesActor->GetTitleTextProperty(0)->GetColor(c);
  d->xAxisTitleColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetTitleTextProperty(1)->GetColor(c);
  d->yAxisTitleColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetTitleTextProperty(2)->GetColor(c);
  d->zAxisTitleColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  d->xAxisTitleFontSizeSpinBox->setValue(axesActor->GetTitleTextProperty(0)->GetFontSize());
  d->yAxisTitleFontSizeSpinBox->setValue(axesActor->GetTitleTextProperty(1)->GetFontSize());
  d->zAxisTitleFontSizeSpinBox->setValue(axesActor->GetTitleTextProperty(2)->GetFontSize());

  d->xAxisTitleBoldCheckBox->setChecked(axesActor->GetTitleTextProperty(0)->GetBold());
  d->xAxisTitleBoldCheckBox->setChecked(axesActor->GetTitleTextProperty(1)->GetBold());
  d->zAxisTitleBoldCheckBox->setChecked(axesActor->GetTitleTextProperty(2)->GetBold());

  d->xAxisTitleItalicCheckBox->setChecked(axesActor->GetTitleTextProperty(0)->GetItalic());
  d->yAxisTitleItalicCheckBox->setChecked(axesActor->GetTitleTextProperty(0)->GetItalic());
  d->zAxisTitleItalicCheckBox->setChecked(axesActor->GetTitleTextProperty(0)->GetItalic());

  // Grid settings
  d->xGridWidthSpinBox->setValue(axesActor->GetXAxesGridlinesProperty()->GetLineWidth());
  d->yGridWidthSpinBox->setValue(axesActor->GetYAxesGridlinesProperty()->GetLineWidth());
  d->zGridWidthSpinBox->setValue(axesActor->GetZAxesGridlinesProperty()->GetLineWidth());

  d->xGridTubeCheckBox->setChecked(
          axesActor->GetXAxesGridlinesProperty()->GetRenderLinesAsTubes());
  d->yGridTubeCheckBox->setChecked(
          axesActor->GetYAxesGridlinesProperty()->GetRenderLinesAsTubes());
  d->zGridTubeCheckBox->setChecked(
          axesActor->GetZAxesGridlinesProperty()->GetRenderLinesAsTubes());

  axesActor->GetXAxesGridlinesProperty()->GetColor(c);
  d->xGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetYAxesGridlinesProperty()->GetColor(c);
  d->yGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetZAxesGridlinesProperty()->GetColor(c);
  d->zGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  // Inner grid settings
  d->xInnerGridWidthSpinBox->setValue(axesActor->GetXAxesInnerGridlinesProperty()->GetLineWidth());
  d->yInnerGridWidthSpinBox->setValue(axesActor->GetYAxesInnerGridlinesProperty()->GetLineWidth());
  d->zInnerGridWidthSpinBox->setValue(axesActor->GetZAxesInnerGridlinesProperty()->GetLineWidth());

  d->xInnerGridTubeCheckBox->setChecked(
          axesActor->GetXAxesInnerGridlinesProperty()->GetRenderLinesAsTubes());
  d->yInnerGridTubeCheckBox->setChecked(
          axesActor->GetYAxesInnerGridlinesProperty()->GetRenderLinesAsTubes());
  d->zInnerGridTubeCheckBox->setChecked(
          axesActor->GetZAxesInnerGridlinesProperty()->GetRenderLinesAsTubes());

  axesActor->GetXAxesInnerGridlinesProperty()->GetColor(c);
  d->xInnerGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetYAxesInnerGridlinesProperty()->GetColor(c);
  d->yInnerGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetZAxesInnerGridlinesProperty()->GetColor(c);
  d->zInnerGridColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  // Grid polys settings
  d->xGridpolysEdgesCheckBox->setChecked(axesActor->GetXAxesGridpolysProperty()->GetEdgeVisibility());
  d->yGridpolysEdgesCheckBox->setChecked(axesActor->GetYAxesGridpolysProperty()->GetEdgeVisibility());
  d->zGridpolysEdgesCheckBox->setChecked(axesActor->GetZAxesGridpolysProperty()->GetEdgeVisibility());

  axesActor->GetXAxesGridpolysProperty()->GetEdgeColor(c);
  d->xGridpolysEdgesColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetYAxesGridpolysProperty()->GetEdgeColor(c);
  d->yGridpolysEdgesColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetZAxesGridpolysProperty()->GetEdgeColor(c);
  d->zGridpolysEdgesColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  d->xGridpolysOpacitySliderWidget->setValue(axesActor->GetXAxesGridpolysProperty()->GetOpacity());
  d->yGridpolysOpacitySliderWidget->setValue(axesActor->GetYAxesGridpolysProperty()->GetOpacity());
  d->zGridpolysOpacitySliderWidget->setValue(axesActor->GetZAxesGridpolysProperty()->GetOpacity());

  axesActor->GetXAxesGridpolysProperty()->GetColor(c);
  d->xGridpolysColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetYAxesGridpolysProperty()->GetColor(c);
  d->yGridpolysColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));
  axesActor->GetZAxesGridpolysProperty()->GetColor(c);
  d->zGridpolysColorBtn->setColor(
        QColor(c[0]*255, c[1]*255, c[2]*255));

  d->showAxesBtn->setChecked(axesActor->GetVisibility());
}

void qSlicerAxes3DWidget::setupAxesFromWidget()
{
  Q_D(qSlicerAxes3DWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  this->qvtkBlockAll();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  vtkMRMLDisplayableNode* dispNode =
      vtkMRMLDisplayableNode::SafeDownCast(
        d->nodeSelector->currentNode());

  if (dispNode)
    axesManager3D->SetAndObserveMRMLNodeBounds(dispNode);

  /*-------- START OF THE SETTINGS --------*/

  axesActor->SetUse2DMode(1);
  axesActor->SetUseTextActor3D(0);
  axesActor->SetCamera(
        axesManager3D->GetRenderer()->GetActiveCamera());

  // General settings
  axesActor->SetXAxisVisibility(d->xAxisCheckBox->isChecked());
  axesActor->SetYAxisVisibility(d->yAxisCheckBox->isChecked());
  axesActor->SetZAxisVisibility(d->zAxisCheckBox->isChecked());

  axesActor->SetXAxisTickVisibility(d->xTicksCheckBox->isChecked());
  axesActor->SetYAxisTickVisibility(d->yTicksCheckBox->isChecked());
  axesActor->SetZAxisTickVisibility(d->zTicksCheckBox->isChecked());

  axesActor->SetXAxisMinorTickVisibility(d->xMinorTicksCheckBox->isChecked());
  axesActor->SetYAxisMinorTickVisibility(d->yMinorTicksCheckBox->isChecked());
  axesActor->SetZAxisMinorTickVisibility(d->zMinorTicksCheckBox->isChecked());

  axesActor->SetXAxisLabelVisibility(d->xLabelCheckBox->isChecked());
  axesActor->SetYAxisLabelVisibility(d->yLabelCheckBox->isChecked());
  axesActor->SetZAxisLabelVisibility(d->zLabelCheckBox->isChecked());

  axesActor->SetDrawXGridlines(d->xGridCheckBox->isChecked());
  axesActor->SetDrawYGridlines(d->yGridCheckBox->isChecked());
  axesActor->SetDrawZGridlines(d->zGridCheckBox->isChecked());

  axesActor->SetDrawXInnerGridlines(d->xInnerGridCheckBox->isChecked());
  axesActor->SetDrawYInnerGridlines(d->yInnerGridCheckBox->isChecked());
  axesActor->SetDrawZInnerGridlines(d->zInnerGridCheckBox->isChecked());

  axesActor->SetDrawXGridpolys(d->xGridpolysCheckBox->isChecked());
  axesActor->SetDrawYGridpolys(d->yGridpolysCheckBox->isChecked());
  axesActor->SetDrawZGridpolys(d->zGridpolysCheckBox->isChecked());

  axesActor->SetFlyMode(d->flyModeCombo->currentIndex());
  axesActor->SetGridLineLocation(d->gridLocationCombo->currentIndex());
  axesActor->SetTickLocation(d->tickLocationCombo->currentIndex());

  // Axes settings
  axesActor->GetXAxesLinesProperty()->SetLineWidth(d->xAxisWidthSpinBox->value());
  axesActor->GetYAxesLinesProperty()->SetLineWidth(d->yAxisWidthSpinBox->value());
  axesActor->GetZAxesLinesProperty()->SetLineWidth(d->zAxisWidthSpinBox->value());

  axesActor->GetXAxesLinesProperty()->SetRenderLinesAsTubes(d->xAxisTubeCheckBox->isChecked());
  axesActor->GetYAxesLinesProperty()->SetRenderLinesAsTubes(d->yAxisTubeCheckBox->isChecked());
  axesActor->GetZAxesLinesProperty()->SetRenderLinesAsTubes(d->zAxisTubeCheckBox->isChecked());

  QColor c;
  c = d->xAxisColorBtn->color();
  axesActor->GetXAxesLinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yAxisColorBtn->color();
  axesActor->GetYAxesLinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zAxisColorBtn->color();
  axesActor->GetZAxesLinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  // Axes labels (numbers) settings
  c = d->xAxisLabelColorBtn->color();
  axesActor->GetLabelTextProperty(0)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yAxisLabelColorBtn->color();
  axesActor->GetLabelTextProperty(1)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zAxisLabelColorBtn->color();
  axesActor->GetLabelTextProperty(2)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  axesActor->GetLabelTextProperty(0)->SetFontSize(d->xAxisLabelFontSizeSpinBox->value());
  axesActor->GetLabelTextProperty(1)->SetFontSize(d->yAxisLabelFontSizeSpinBox->value());
  axesActor->GetLabelTextProperty(2)->SetFontSize(d->zAxisLabelFontSizeSpinBox->value());

  axesActor->GetLabelTextProperty(0)->SetLineOffset(d->xAxisLabelOffsetSpinBox->value());
  axesActor->GetLabelTextProperty(1)->SetLineOffset(d->yAxisLabelOffsetSpinBox->value());
  axesActor->GetLabelTextProperty(2)->SetLineOffset(d->zAxisLabelOffsetSpinBox->value());

  axesActor->GetLabelTextProperty(0)->SetBold(d->xAxisLabelBoldCheckBox->isChecked());
  axesActor->GetLabelTextProperty(1)->SetBold(d->yAxisLabelBoldCheckBox->isChecked());
  axesActor->GetLabelTextProperty(2)->SetBold(d->zAxisLabelBoldCheckBox->isChecked());

  axesActor->GetLabelTextProperty(0)->SetItalic(d->xAxisLabelItalicCheckBox->isChecked());
  axesActor->GetLabelTextProperty(1)->SetItalic(d->yAxisLabelItalicCheckBox->isChecked());
  axesActor->GetLabelTextProperty(2)->SetItalic(d->zAxisLabelItalicCheckBox->isChecked());

  // Axes titles settings
  axesActor->SetXTitle(d->xAxisTitleLineEdit->text().toUtf8());
  axesActor->SetYTitle(d->yAxisTitleLineEdit->text().toUtf8());
  axesActor->SetZTitle(d->zAxisTitleLineEdit->text().toUtf8());

  axesActor->GetTitleTextProperty(0)->SetLineOffset(d->xAxisTitleOffsetSpinBox->value());
  axesActor->GetTitleTextProperty(1)->SetLineOffset(d->yAxisTitleOffsetSpinBox->value());
  axesActor->GetTitleTextProperty(2)->SetLineOffset(d->zAxisTitleOffsetSpinBox->value());

  c = d->xAxisTitleColorBtn->color();
  axesActor->GetTitleTextProperty(0)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yAxisTitleColorBtn->color();
  axesActor->GetTitleTextProperty(1)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zAxisTitleColorBtn->color();
  axesActor->GetTitleTextProperty(2)->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  axesActor->GetTitleTextProperty(0)->SetFontSize(d->xAxisTitleFontSizeSpinBox->value());
  axesActor->GetTitleTextProperty(1)->SetFontSize(d->yAxisTitleFontSizeSpinBox->value());
  axesActor->GetTitleTextProperty(2)->SetFontSize(d->zAxisTitleFontSizeSpinBox->value());

  axesActor->GetTitleTextProperty(0)->SetBold(d->xAxisTitleBoldCheckBox->isChecked());
  axesActor->GetTitleTextProperty(1)->SetBold(d->yAxisTitleBoldCheckBox->isChecked());
  axesActor->GetTitleTextProperty(2)->SetBold(d->zAxisTitleBoldCheckBox->isChecked());

  axesActor->GetTitleTextProperty(0)->SetItalic(d->xAxisTitleItalicCheckBox->isChecked());
  axesActor->GetTitleTextProperty(1)->SetItalic(d->yAxisTitleItalicCheckBox->isChecked());
  axesActor->GetTitleTextProperty(2)->SetItalic(d->zAxisTitleItalicCheckBox->isChecked());

  // Grid settings
  axesActor->GetXAxesGridlinesProperty()->SetLineWidth(d->xGridWidthSpinBox->value());
  axesActor->GetYAxesGridlinesProperty()->SetLineWidth(d->yGridWidthSpinBox->value());
  axesActor->GetZAxesGridlinesProperty()->SetLineWidth(d->zGridWidthSpinBox->value());

  axesActor->GetXAxesGridlinesProperty()->SetRenderLinesAsTubes(d->xGridTubeCheckBox->isChecked());
  axesActor->GetYAxesGridlinesProperty()->SetRenderLinesAsTubes(d->yGridTubeCheckBox->isChecked());
  axesActor->GetZAxesGridlinesProperty()->SetRenderLinesAsTubes(d->zGridTubeCheckBox->isChecked());

  c = d->xGridColorBtn->color();
  axesActor->GetXAxesGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yGridColorBtn->color();
  axesActor->GetYAxesGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zGridColorBtn->color();
  axesActor->GetZAxesGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  // Inner grid settings
  axesActor->GetXAxesInnerGridlinesProperty()->SetLineWidth(d->xInnerGridWidthSpinBox->value());
  axesActor->GetYAxesInnerGridlinesProperty()->SetLineWidth(d->yInnerGridWidthSpinBox->value());
  axesActor->GetZAxesInnerGridlinesProperty()->SetLineWidth(d->zInnerGridWidthSpinBox->value());

  axesActor->GetXAxesInnerGridlinesProperty()->SetRenderLinesAsTubes(d->xInnerGridTubeCheckBox->isChecked());
  axesActor->GetYAxesInnerGridlinesProperty()->SetRenderLinesAsTubes(d->yInnerGridTubeCheckBox->isChecked());
  axesActor->GetZAxesInnerGridlinesProperty()->SetRenderLinesAsTubes(d->zInnerGridTubeCheckBox->isChecked());

  c = d->xInnerGridColorBtn->color();
  axesActor->GetXAxesInnerGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yInnerGridColorBtn->color();
  axesActor->GetYAxesInnerGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zInnerGridColorBtn->color();
  axesActor->GetZAxesInnerGridlinesProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  // Grid polys settings
  axesActor->GetXAxesGridpolysProperty()->SetEdgeVisibility(d->xGridpolysEdgesCheckBox->isChecked());
  axesActor->GetYAxesGridpolysProperty()->SetEdgeVisibility(d->yGridpolysEdgesCheckBox->isChecked());
  axesActor->GetZAxesGridpolysProperty()->SetEdgeVisibility(d->zGridpolysEdgesCheckBox->isChecked());

  c = d->xGridpolysEdgesColorBtn->color();
  axesActor->GetXAxesGridpolysProperty()->SetEdgeColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yGridpolysEdgesColorBtn->color();
  axesActor->GetYAxesGridpolysProperty()->SetEdgeColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zGridpolysEdgesColorBtn->color();
  axesActor->GetZAxesGridpolysProperty()->SetEdgeColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  axesActor->GetXAxesGridpolysProperty()->SetOpacity(d->xGridpolysOpacitySliderWidget->value());
  axesActor->GetYAxesGridpolysProperty()->SetOpacity(d->yGridpolysOpacitySliderWidget->value());
  axesActor->GetZAxesGridpolysProperty()->SetOpacity(d->zGridpolysOpacitySliderWidget->value());

  c = d->xGridpolysColorBtn->color();
  axesActor->GetXAxesGridpolysProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->yGridpolysColorBtn->color();
  axesActor->GetYAxesGridpolysProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);
  c = d->zGridpolysColorBtn->color();
  axesActor->GetZAxesGridpolysProperty()->SetColor(
        (double)c.red()/(double)255, (double)c.green()/(double)255, (double)c.blue()/(double)255);

  /*-------- END OF THE SETTINGS --------*/
  this->qvtkUnblockAll();
  axesManager3D->RequestRender();
}
