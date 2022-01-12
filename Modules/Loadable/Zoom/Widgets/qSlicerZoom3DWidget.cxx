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

// 3D Widgets includes
#include "qSlicerZoom3DWidget.h"
#include "ui_qSlicerZoom3DWidget.h"
#include "vtkSlicerZoomLogic.h"

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// MRML includes
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLCameraNode.h"

// VTK includes
#include <vtkCamera.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Zoom
class qSlicerZoom3DWidgetPrivate
  : public Ui_qSlicerZoom3DWidget
{
  Q_DECLARE_PUBLIC(qSlicerZoom3DWidget);
protected:
  qSlicerZoom3DWidget* const q_ptr;

public:
  qSlicerZoom3DWidgetPrivate(
    qSlicerZoom3DWidget& object);
  virtual void setupUi(qSlicerZoom3DWidget*);

  qSlicerApplication* app = nullptr;
  vtkSlicerZoomLogic* logic = nullptr;
};

// --------------------------------------------------------------------------
qSlicerZoom3DWidgetPrivate
::qSlicerZoom3DWidgetPrivate(
  qSlicerZoom3DWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerZoom3DWidgetPrivate
::setupUi(qSlicerZoom3DWidget* widget)
{
  this->Ui_qSlicerZoom3DWidget::setupUi(widget);

  app = qSlicerApplication::application();
  if (app)
    logic = vtkSlicerZoomLogic::SafeDownCast(
          app->moduleLogic("Zoom"));
}

//-----------------------------------------------------------------------------
// qSlicerZoom3DWidget methods

//-----------------------------------------------------------------------------
qSlicerZoom3DWidget
::qSlicerZoom3DWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerZoom3DWidgetPrivate(*this) )
{
  Q_D(qSlicerZoom3DWidget);
  d->setupUi(this);

  d->xGainWidget->setMinimum(0);
  d->yGainWidget->setMinimum(0);
  d->zGainWidget->setMinimum(0);

  connect(d->viewNodeSelector, qOverload<vtkMRMLNode*>(&qMRMLNodeComboBox::currentNodeChanged),
          this, &qSlicerZoom3DWidget::onCurrentViewNodeChanged);

  connect(d->xGainWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoom3DWidget::onViewGainChanged);
  connect(d->yGainWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoom3DWidget::onViewGainChanged);
  connect(d->zGainWidget, qOverload<double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoom3DWidget::onViewGainChanged);
}

//-----------------------------------------------------------------------------
qSlicerZoom3DWidget
::~qSlicerZoom3DWidget()
{
}

void qSlicerZoom3DWidget::onCurrentViewNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerZoom3DWidget);
  vtkMRMLViewNode* viewNode = vtkMRMLViewNode::SafeDownCast(node);
  if (!viewNode)
    return;

  if (!d->logic)
    return;

  if (!qvtkIsConnected(d->logic->GetCamera(viewNode), vtkCommand::ModifiedEvent,
                       this, SLOT(onViewNodeModified(vtkObject*)))){
    qvtkConnect(d->logic->GetCamera(viewNode), vtkCommand::ModifiedEvent,
                this, SLOT(onViewNodeModified(vtkObject*)));
  }

  setupWidgetFromZoom();
}

void qSlicerZoom3DWidget::onViewGainChanged(double gain)
{
  setupZoomFromWidget();
}


void qSlicerZoom3DWidget::onViewNodeModified(vtkObject* sender)
{
  setupWidgetFromZoom();
}

void qSlicerZoom3DWidget::setupWidgetFromZoom()
{
  Q_D(qSlicerZoom3DWidget);
  vtkMRMLViewNode* viewNode =
      vtkMRMLViewNode::SafeDownCast(d->viewNodeSelector->currentNode());

  if (!viewNode)
    return;

  if (!d->logic)
    return;

  double xyz[3];
  d->logic->GetViewScale(viewNode, xyz);

  d->xGainWidget->blockSignals(true);
  d->xGainWidget->setGain(xyz[0]);
  d->xGainWidget->blockSignals(false);

  d->yGainWidget->blockSignals(true);
  d->yGainWidget->setGain(xyz[1]);
  d->yGainWidget->blockSignals(false);

  d->zGainWidget->blockSignals(true);
  d->zGainWidget->setGain(xyz[2]);
  d->zGainWidget->blockSignals(false);
}

void qSlicerZoom3DWidget::setupZoomFromWidget()
{
  Q_D(qSlicerZoom3DWidget);
  vtkMRMLViewNode* viewNode =
      vtkMRMLViewNode::SafeDownCast(d->viewNodeSelector->currentNode());

  if (!viewNode)
    return;

  double xGain, yGain, zGain;
  xGain = d->xGainWidget->getGain() > 0 ?
        d->xGainWidget->getGain() : 1;
  yGain = d->yGainWidget->getGain() > 0 ?
        d->yGainWidget->getGain() : 1;
  zGain = d->zGainWidget->getGain() > 0 ?
        d->zGainWidget->getGain() : 1;

  if (!d->logic)
    return;

  this->qvtkBlockAll();
  d->logic->ScaleView(viewNode, xGain, yGain, zGain);
  this->qvtkUnblockAll();
}
