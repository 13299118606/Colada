/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// Slicer includes
#include "qSlicerAxesModuleWidget.h"
#include "ui_qSlicerAxesModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// VTK includes
#include "vtkCubeAxesActor.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLAxesDisplayableManager2D.h"
#include "vtkMRMLAxesDisplayableManager3D.h"
#include "vtkMRMLScalarVolumeNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerAxesModuleWidgetPrivate: public Ui_qSlicerAxesModuleWidget
{
public:
  qSlicerAxesModuleWidgetPrivate();

  vtkMRMLAxesDisplayableManager2D*
  getCurrentAxesDisplayableManager2D();

  vtkMRMLAxesDisplayableManager3D*
  getCurrentAxesDisplayableManager3D();
};

//-----------------------------------------------------------------------------
// qSlicerAxesModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerAxesModuleWidgetPrivate::qSlicerAxesModuleWidgetPrivate()
{
}

vtkMRMLAxesDisplayableManager2D*
qSlicerAxesModuleWidgetPrivate::getCurrentAxesDisplayableManager2D()
{
  qSlicerApplication* app = qSlicerApplication::application();
  if (!sliceNode2DSelector->currentNode())
    return nullptr;

  char* name = sliceNode2DSelector->currentNode()->GetName();
  qMRMLSliceWidget* sliceWidget = app->layoutManager()->sliceWidget(
        QString::fromUtf8(name));

  if (!sliceWidget)
    return nullptr;

  return vtkMRMLAxesDisplayableManager2D::SafeDownCast(
        sliceWidget->sliceView()->
        displayableManagerByClassName(
          "vtkMRMLAxesDisplayableManager2D"));
}

vtkMRMLAxesDisplayableManager3D*
qSlicerAxesModuleWidgetPrivate::getCurrentAxesDisplayableManager3D()
{
  qSlicerApplication* app = qSlicerApplication::application();
  if (!viewNode3DSelector->currentNode())
    return nullptr;

  char* name = viewNode3DSelector->currentNode()->GetName();
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
// qSlicerAxesModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerAxesModuleWidget::qSlicerAxesModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerAxesModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerAxesModuleWidget::~qSlicerAxesModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerAxesModuleWidget::setup()
{
  Q_D(qSlicerAxesModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  this->setupMRMLAxesDisplayableManager2D();
  this->setupMRMLAxesDisplayableManager3D();
}

void qSlicerAxesModuleWidget::setupMRMLAxesDisplayableManager2D()
{
  Q_D(qSlicerAxesModuleWidget);

  connect(d->sliceNode2DSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onCurrentViewNode2DChanged(vtkMRMLNode*)));
  connect(d->updateAxes2DBtn, &QAbstractButton::clicked,
          this, &qSlicerAxesModuleWidget::onUpdateAxes2DBtnclicked);
  connect(d->showAxes2DBtn, &QAbstractButton::toggled,
          this, &qSlicerAxesModuleWidget::onShowAxes2DBtnToggled);

  // initialize axes
  onUpdateAxes2DBtnclicked();
}

void qSlicerAxesModuleWidget::setupMRMLAxesDisplayableManager3D()
{
  Q_D(qSlicerAxesModuleWidget);

  connect(d->viewNode3DSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
          this, SLOT(onCurrentViewNode3DChanged(vtkMRMLNode*)));
  connect(d->updateAxes3DBtn, &QAbstractButton::clicked,
          this, &qSlicerAxesModuleWidget::onUpdateAxes3DBtnclicked);
  connect(d->showAxes3DBtn, &QAbstractButton::toggled,
          this, &qSlicerAxesModuleWidget::onShowAxes3DBtnToggled);

  // initialize axes
  onUpdateAxes3DBtnclicked();
}

void qSlicerAxesModuleWidget::updateAxes2DSettings()
{
//  Q_D(qSlicerAxesModuleWidget);
//  vtkMRMLAxesDisplayableManager2D* axesManager2D = d->getCurrentAxesDisplayableManager2D();

//  if (!axesManager2D)
//      return;

//  vtkCubeAxesActor2D* axesActor =
//      axesManager2D->GetCubeAxesActor();

//  if (!axesActor)
//    return;

//  vtkMRMLScalarVolumeNode* volumeNode =
//      vtkMRMLScalarVolumeNode::SafeDownCast(
//        d->node2DSelector->currentNode());

//  if (!volumeNode)
//    return;

//  double bounds[6];
////  axesActor->SetInputConnection(volumeNode->GetImageData()->Get)
////  volumeNode->GetImageData()->
////  volumeNode->GetBounds(bounds);
////  axesActor->SetBounds(bounds);
//  axesActor->SetBounds(0, 100, 0, 100, 0, 100);
//  axesActor->SetCamera(
//        axesManager2D->GetRenderer()->GetActiveCamera());

//  axesActor->SetXAxisVisibility(d->xAxis2DCheckBox->checkState());
//  axesActor->SetYAxisVisibility(d->yAxis2DCheckBox->checkState());
//  axesActor->SetZAxisVisibility(d->zAxis2DCheckBox->checkState());
}

void qSlicerAxesModuleWidget::updateAxes3DSettings()
{
  Q_D(qSlicerAxesModuleWidget);
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
        d->node3DSelector->currentNode());

  if (dispNode)
    axesManager3D->SetAndObserveMRMLNodeBounds(dispNode);

  axesActor->SetUse2DMode(1);
  axesActor->SetUseTextActor3D(0);
  axesActor->SetCamera(
        axesManager3D->GetRenderer()->GetActiveCamera());

  axesActor->SetXAxisTickVisibility(d->xTicks3DCheckBox->checkState());
  axesActor->SetYAxisTickVisibility(d->yTicks3DCheckBox->checkState());
  axesActor->SetZAxisTickVisibility(d->zTicks3DCheckBox->checkState());

  axesActor->SetXAxisMinorTickVisibility(d->xMinorTicks3DCheckBox->checkState());
  axesActor->SetYAxisMinorTickVisibility(d->yMinorTicks3DCheckBox->checkState());
  axesActor->SetZAxisMinorTickVisibility(d->zMinorTicks3DCheckBox->checkState());

  axesActor->SetXAxisLabelVisibility(d->xLabel3DCheckBox->checkState());
  axesActor->SetYAxisLabelVisibility(d->yLabel3DCheckBox->checkState());
  axesActor->SetZAxisLabelVisibility(d->zLabel3DCheckBox->checkState());

  axesActor->SetDrawXGridlines(d->xGrid3DCheckBox->checkState());
  axesActor->SetDrawYGridlines(d->yGrid3DCheckBox->checkState());
  axesActor->SetDrawZGridlines(d->zGrid3DCheckBox->checkState());

  this->qvtkUnblockAll();
}

void qSlicerAxesModuleWidget::
setupWidgetSettingsFromMRMLAxesDisplayableManager2D()
{
//  Q_D(qSlicerAxesModuleWidget);
//  vtkMRMLAxesDisplayableManager2D* axesManager2D = d->getCurrentAxesDisplayableManager2D();

//  if (!axesManager2D)
//      return;

//  vtkCubeAxesActor2D* axesActor =
//      axesManager2D->GetCubeAxesActor();

//  if (!axesActor)
//    return;

//  d->xAxis2DCheckBox->setCheckState(
//        axesActor->GetXAxisVisibility() ?
//        Qt::Checked : Qt::Unchecked);
//  d->yAxis2DCheckBox->setCheckState(
//        axesActor->GetYAxisVisibility() ?
//        Qt::Checked : Qt::Unchecked);
//  d->zAxis2DCheckBox->setCheckState(
//        axesActor->GetZAxisVisibility() ?
//        Qt::Checked : Qt::Unchecked);

//  d->showAxes2DBtn->blockSignals(true);
//  d->showAxes2DBtn->setChecked(axesActor->GetVisibility());
//  d->showAxes2DBtn->blockSignals(false);
}

void qSlicerAxesModuleWidget::
setupWidgetSettingsFromMRMLAxesDisplayableManager3D()
{
  Q_D(qSlicerAxesModuleWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  d->xTicks3DCheckBox->setCheckState(
          axesActor->GetXAxisTickVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->yTicks3DCheckBox->setCheckState(
          axesActor->GetYAxisTickVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->zTicks3DCheckBox->setCheckState(
          axesActor->GetZAxisTickVisibility() ?
          Qt::Checked : Qt::Unchecked);

  d->xMinorTicks3DCheckBox->setCheckState(
          axesActor->GetXAxisMinorTickVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->yMinorTicks3DCheckBox->setCheckState(
          axesActor->GetYAxisMinorTickVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->zMinorTicks3DCheckBox->setCheckState(
          axesActor->GetZAxisMinorTickVisibility() ?
          Qt::Checked : Qt::Unchecked);

  d->xLabel3DCheckBox->setCheckState(
          axesActor->GetXAxisLabelVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->yLabel3DCheckBox->setCheckState(
          axesActor->GetYAxisLabelVisibility() ?
          Qt::Checked : Qt::Unchecked);
  d->zLabel3DCheckBox->setCheckState(
          axesActor->GetZAxisLabelVisibility() ?
          Qt::Checked : Qt::Unchecked);

  d->xGrid3DCheckBox->setCheckState(
          axesActor->GetDrawXGridlines() ?
          Qt::Checked : Qt::Unchecked);
  d->yGrid3DCheckBox->setCheckState(
          axesActor->GetDrawYGridlines() ?
          Qt::Checked : Qt::Unchecked);
  d->zGrid3DCheckBox->setCheckState(
          axesActor->GetDrawZGridlines() ?
          Qt::Checked : Qt::Unchecked);

  d->showAxes3DBtn->blockSignals(true);
  d->showAxes3DBtn->setChecked(axesActor->GetVisibility());
  d->showAxes3DBtn->blockSignals(false);
}

void qSlicerAxesModuleWidget::onCurrentViewNode2DChanged(vtkMRMLNode* node)
{
//  Q_D(qSlicerAxesModuleWidget);
//  vtkMRMLAxesDisplayableManager2D* axesManager2D = d->getCurrentAxesDisplayableManager2D();

//  if (!axesManager2D)
//      return;

//  vtkCubeAxesActor2D* axesActor =
//      axesManager2D->GetCubeAxesActor();

//  if (!axesActor)
//    return;

//  setupWidgetSettingsFromMRMLAxesDisplayableManager2D();
}

void qSlicerAxesModuleWidget::onCurrentViewNode3DChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerAxesModuleWidget);
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
  }

  setupWidgetSettingsFromMRMLAxesDisplayableManager3D();
}

void qSlicerAxesModuleWidget::onUpdateAxes2DBtnclicked()
{
//  Q_D(qSlicerAxesModuleWidget);
//  vtkMRMLAxesDisplayableManager2D* axesManager2D = d->getCurrentAxesDisplayableManager2D();

//  if (!axesManager2D)
//      return;

//  vtkCubeAxesActor2D* axesActor =
//      axesManager2D->GetCubeAxesActor();

//  if (!axesActor)
//    return;

//  updateAxes2DSettings();
//  if (axesActor->GetVisibility())
//    axesManager2D->RequestRender();
}

void qSlicerAxesModuleWidget::onUpdateAxes3DBtnclicked()
{
  Q_D(qSlicerAxesModuleWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  updateAxes3DSettings();
  axesManager3D->RequestRender();
}

void qSlicerAxesModuleWidget::onShowAxes2DBtnToggled(bool val)
{
//  Q_D(qSlicerAxesModuleWidget);
//  vtkMRMLAxesDisplayableManager2D* axesManager2D = d->getCurrentAxesDisplayableManager2D();

//  if (!axesManager2D)
//      return;

//  vtkCubeAxesActor2D* axesActor =
//      axesManager2D->GetCubeAxesActor();

//  if (!axesActor)
//    return;

//  axesActor->SetVisibility(val);
//  if (val){
//    updateAxes2DSettings();
//    axesManager2D->RequestRender();
//  }
}

void qSlicerAxesModuleWidget::onShowAxes3DBtnToggled(bool val)
{
  Q_D(qSlicerAxesModuleWidget);
  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  if (!axesActor)
    return;

  axesActor->SetVisibility(val);
  updateAxes3DSettings();
  axesManager3D->RequestRender();
}

void qSlicerAxesModuleWidget::onCubeAxesActorModified(vtkObject* sender)
{
  Q_D(qSlicerAxesModuleWidget);
  vtkCubeAxesActor* axesActorSender =
      vtkCubeAxesActor::SafeDownCast(sender);

  vtkMRMLAxesDisplayableManager3D* axesManager3D = d->getCurrentAxesDisplayableManager3D();

  if (!axesManager3D)
      return;

  vtkCubeAxesActor* axesActor =
      axesManager3D->GetCubeAxesActor();

  // check if the sender belongs to the current `viewNode3DSelector` node
  // if not then we must not change values in the displayed widgets
  if (!axesActorSender ||
      !axesActor ||
      axesActorSender != axesActor)
    return;

  setupWidgetSettingsFromMRMLAxesDisplayableManager3D();
}
