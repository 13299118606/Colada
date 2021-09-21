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
#include "qSlicerZoomModuleWidget.h"
#include "ui_qSlicerZoomModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLCameraNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"

// Slicer logic includes
#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerCamerasModuleLogic.h"

// VTK includes
#include <vtkRenderer.h>
#include <vtkTransform.h>
#include <vtkCamera.h>

// CTK includes
#include <ctkDoubleSlider.h>

// logic includes
#include "vtkSlicerZoomLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerZoomModuleWidgetPrivate: public Ui_qSlicerZoomModuleWidget
{
  Q_DECLARE_PUBLIC(qSlicerZoomModuleWidget);

protected:
  qSlicerZoomModuleWidget* q_ptr;

public:
  qSlicerZoomModuleWidgetPrivate(qSlicerZoomModuleWidget& object);

protected:
  /// return `true` to stop the event, `false` - to process the event
  bool eventFilter(QObject *object, QEvent *event)
  {
    Q_Q(qSlicerZoomModuleWidget);

    if (event->type() != QEvent::Resize)
      return false;

    qMRMLSliceWidget* sliceWidgetWatched =
        qobject_cast<qMRMLSliceWidget*>(object);

    if (!sliceWidgetWatched)
      return false;

    vtkSlicerZoomLogic* zoomLogic =
        vtkSlicerZoomLogic::SafeDownCast(q->logic());

    if (!zoomLogic)
      return false;

    vtkMRMLSliceNode* sliceNode =
        vtkMRMLSliceNode::SafeDownCast(sliceNode2DSelector->currentNode());

    qMRMLSliceWidget* sliceWidget =
        zoomLogic->GetSliceWidget(sliceNode);

    if (sliceWidgetWatched)
      return true;

    return false;
  }
};

//-----------------------------------------------------------------------------
// qSlicerZoomModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerZoomModuleWidgetPrivate::qSlicerZoomModuleWidgetPrivate(qSlicerZoomModuleWidget& object)
  : q_ptr()
{
}

//-----------------------------------------------------------------------------
// qSlicerZoomModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerZoomModuleWidget::qSlicerZoomModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerZoomModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerZoomModuleWidget::~qSlicerZoomModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerZoomModuleWidget::setup()
{
  Q_D(qSlicerZoomModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  d->xGain2DWidget->setMinimum(0);
  d->yGain2DWidget->setMinimum(0);

  d->xGain3DWidget->setMinimum(0);
  d->yGain3DWidget->setMinimum(0);
  d->zGain3DWidget->setMinimum(0);

  connect(d->sliceNode2DSelector, qOverload<vtkMRMLNode*>(&qMRMLNodeComboBox::currentNodeChanged),
          this, &qSlicerZoomModuleWidget::onCurrentViewNode2DChanged);
  connect(d->viewNode3DSelector, qOverload<vtkMRMLNode*>(&qMRMLNodeComboBox::currentNodeChanged),
          this, &qSlicerZoomModuleWidget::onCurrentViewNode3DChanged);

  connect(d->xGain2DWidget, qOverload<double, double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoomModuleWidget::on2DSliceGainChanged);
  connect(d->yGain2DWidget, qOverload<double, double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoomModuleWidget::on2DSliceGainChanged);

  connect(d->xGain3DWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoomModuleWidget::on3DViewGainChanged);
  connect(d->yGain3DWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoomModuleWidget::on3DViewGainChanged);
  connect(d->zGain3DWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoomModuleWidget::on3DViewGainChanged);
}

void qSlicerZoomModuleWidget::onCurrentViewNode2DChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerZoomModuleWidget);
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  if (!sliceNode)
    return;

  if (!qvtkIsConnected(sliceNode, vtkCommand::ModifiedEvent,
                       this, SLOT(onSliceNodeModified(vtkObject*)))){
    qvtkConnect(sliceNode, vtkCommand::ModifiedEvent,
                this, SLOT(onSliceNodeModified(vtkObject*)));
  }

  setupWidgetSettingsFromSliceNode(sliceNode);

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  zoomLogic->GetSliceWidget(sliceNode)->installEventFilter(this);

//  disconnect(zoomLogic->GetSliceWidget(sliceNode)->sliceView(), SIGNAL(resized),
//             zoomLogic->GetSliceWidget(sliceNode)->sliceView(), nullptr);
//  disconnect(zoomLogic->GetSliceWidget(sliceNode)->sliceView(), nullptr,
//             nullptr, nullptr);
//  zoomLogic->GetSliceWidget(sliceNode)->sliceView()->disconnect(
//        zoomLogic->GetSliceWidget(sliceNode)->sliceView());
  disconnect(zoomLogic->GetSliceWidget(sliceNode)->sliceView(), &qMRMLSliceView::resized,
             nullptr, nullptr);

  connect(zoomLogic->GetSliceWidget(sliceNode)->sliceView(), &qMRMLSliceView::resized,
          this, &qSlicerZoomModuleWidget::onSliceViewResized);
}

void qSlicerZoomModuleWidget::onCurrentViewNode3DChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerZoomModuleWidget);
  vtkMRMLViewNode* viewNode = vtkMRMLViewNode::SafeDownCast(node);
  if (!viewNode)
    return;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  if (!qvtkIsConnected(zoomLogic->GetCamera(viewNode), vtkCommand::ModifiedEvent,
                       this, SLOT(onView3DNodeModified(vtkObject*)))){
    qvtkConnect(zoomLogic->GetCamera(viewNode), vtkCommand::ModifiedEvent,
                this, SLOT(onView3DNodeModified(vtkObject*)));
  }

  setupWidgetSettingsFromView3DNode(viewNode);
}

void qSlicerZoomModuleWidget::on2DSliceGainChanged(double oldGain, double newGain)
{
  Q_D(qSlicerZoomModuleWidget);
  qSlicerGainWidget* gainWidgetSender =
      qobject_cast<qSlicerGainWidget*>(sender());

  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNode2DSelector->currentNode());

  double xGain, yGain,
      xFieldOfView, yFieldOfView, zFieldOfView;
  xGain = d->xGain2DWidget->getGain() > 0 ?
        d->xGain2DWidget->getGain() : 1;
  yGain = d->yGain2DWidget->getGain() > 0 ?
        d->yGain2DWidget->getGain() : 1;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  this->qvtkBlockAll();
  zoomLogic->ScaleSlice(sliceNode, xGain, yGain);
  this->qvtkUnblockAll();
}

void qSlicerZoomModuleWidget::on3DViewGainChanged(double gain)
{
  Q_D(qSlicerZoomModuleWidget);

  vtkMRMLViewNode* viewNode =
      vtkMRMLViewNode::SafeDownCast(d->viewNode3DSelector->currentNode());

  if (!viewNode)
    return;

  double xGain, yGain, zGain;
  xGain = d->xGain3DWidget->getGain() > 0 ?
        d->xGain3DWidget->getGain() : 1;
  yGain = d->yGain3DWidget->getGain() > 0 ?
        d->yGain3DWidget->getGain() : 1;
  zGain = d->zGain3DWidget->getGain() > 0 ?
        d->zGain3DWidget->getGain() : 1;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  this->qvtkBlockAll();
  zoomLogic->ScaleView(viewNode, xGain, yGain, zGain);
  this->qvtkUnblockAll();
}

void qSlicerZoomModuleWidget::setupWidgetSettingsFromSliceNode(vtkMRMLSliceNode *sliceNode)
{
  Q_D(qSlicerZoomModuleWidget);
  if (!sliceNode)
    return;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  double xyz[3];
  zoomLogic->GetSliceScale(sliceNode, xyz);

  d->xGain2DWidget->blockSignals(true);
  d->xGain2DWidget->setGain(xyz[0]);
  d->xGain2DWidget->blockSignals(false);

  d->yGain2DWidget->blockSignals(true);
  d->yGain2DWidget->setGain(xyz[1]);
  d->yGain2DWidget->blockSignals(false);
}

void qSlicerZoomModuleWidget::setupWidgetSettingsFromView3DNode(vtkMRMLViewNode* viewNode)
{
  Q_D(qSlicerZoomModuleWidget);
  if (!viewNode)
    return;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  double xyz[3];
  zoomLogic->GetViewScale(viewNode, xyz);

  d->xGain3DWidget->blockSignals(true);
  d->xGain3DWidget->setGain(xyz[0]);
  d->xGain3DWidget->blockSignals(false);

  d->yGain3DWidget->blockSignals(true);
  d->yGain3DWidget->setGain(xyz[1]);
  d->yGain3DWidget->blockSignals(false);

  d->zGain3DWidget->blockSignals(true);
  d->zGain3DWidget->setGain(xyz[2]);
  d->zGain3DWidget->blockSignals(false);
}

void qSlicerZoomModuleWidget::onSliceNodeModified(vtkObject* sender)
{
  Q_D(qSlicerZoomModuleWidget);
  vtkMRMLSliceNode* sliceNodeSender =
      vtkMRMLSliceNode::SafeDownCast(sender);

  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNode2DSelector->currentNode());

  // check if the sender belongs to the current `viewNode3DSelector` node
  // if not then we must not change values in the displayed widgets
  if (!sliceNodeSender ||
      !sliceNode ||
      sliceNodeSender != sliceNode)
    return;

  setupWidgetSettingsFromSliceNode(sliceNode);
}

void qSlicerZoomModuleWidget::onView3DNodeModified(vtkObject* sender)
{
  Q_D(qSlicerZoomModuleWidget);
  vtkMRMLViewNode* viewNode =
      vtkMRMLViewNode::SafeDownCast(d->viewNode3DSelector->currentNode());

  if (!viewNode)
    return;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  vtkCamera* cameraSender = vtkCamera::SafeDownCast(sender);
  vtkCamera* camera = zoomLogic->GetCamera(viewNode);

  // check if the sender belongs to the current `viewNode3DSelector` node
  // if not then we must not change values in the displayed widgets
  if (!cameraSender ||
      !camera ||
      cameraSender != camera)
    return;

  setupWidgetSettingsFromView3DNode(viewNode);
}

/// return `true` to stop the event, `false` - to process the event
bool qSlicerZoomModuleWidget::eventFilter(QObject *object, QEvent *event)
{
  Q_D(qSlicerZoomModuleWidget);

  if (event->type() != QEvent::Resize)
    return false;

  qMRMLSliceWidget* sliceWidgetWatched =
      qobject_cast<qMRMLSliceWidget*>(object);

  if (!sliceWidgetWatched)
    return false;

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return false;

  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNode2DSelector->currentNode());

  qMRMLSliceWidget* sliceWidget =
      zoomLogic->GetSliceWidget(sliceNode);

  if (sliceWidgetWatched)
    return true;

  return false;
}

void qSlicerZoomModuleWidget::onSliceViewResized(const QSize& newSize)
{
  Q_D(qSlicerZoomModuleWidget);

  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNode2DSelector->currentNode());

  zoomLogic->ResizeSliceNode(sliceNode, newSize.width(), newSize.height());
}
