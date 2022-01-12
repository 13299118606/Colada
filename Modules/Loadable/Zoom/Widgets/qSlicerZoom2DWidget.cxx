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

// 2D Widgets includes
#include "qSlicerZoom2DWidget.h"
#include "ui_qSlicerZoom2DWidget.h"
#include "vtkSlicerZoomLogic.h"

// Slicer includes
#include "qSlicerApplication.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLScene.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Zoom
class qSlicerZoom2DWidgetPrivate
  : public Ui_qSlicerZoom2DWidget
{
  Q_DECLARE_PUBLIC(qSlicerZoom2DWidget);
protected:
  qSlicerZoom2DWidget* const q_ptr;

public:
  qSlicerZoom2DWidgetPrivate(
    qSlicerZoom2DWidget& object);
  virtual void setupUi(qSlicerZoom2DWidget*);

  qSlicerApplication* app = nullptr;
  vtkSlicerZoomLogic* logic = nullptr;
};

// --------------------------------------------------------------------------
qSlicerZoom2DWidgetPrivate
::qSlicerZoom2DWidgetPrivate(
  qSlicerZoom2DWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerZoom2DWidgetPrivate
::setupUi(qSlicerZoom2DWidget* widget)
{
  this->Ui_qSlicerZoom2DWidget::setupUi(widget);

  app = qSlicerApplication::application();
  if (app)
    logic = vtkSlicerZoomLogic::SafeDownCast(
          app->moduleLogic("Zoom"));
}

//-----------------------------------------------------------------------------
// qSlicerZoom2DWidget methods

//-----------------------------------------------------------------------------
qSlicerZoom2DWidget
::qSlicerZoom2DWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerZoom2DWidgetPrivate(*this) )
{
  Q_D(qSlicerZoom2DWidget);
  d->setupUi(this);

  d->xGainWidget->setMinimum(0);
  d->yGainWidget->setMinimum(0);

  connect(d->sliceNodeSelector, qOverload<vtkMRMLNode*>(&qMRMLNodeComboBox::currentNodeChanged),
          this, &qSlicerZoom2DWidget::onCurrentSliceNodeChanged);

  connect(d->xGainWidget, qOverload<double, double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoom2DWidget::onSliceGainChanged);
  connect(d->yGainWidget, qOverload<double, double>(&qSlicerGainWidget::gainChanged),
          this, &qSlicerZoom2DWidget::onSliceGainChanged);
}

//-----------------------------------------------------------------------------
qSlicerZoom2DWidget
::~qSlicerZoom2DWidget()
{
}

void qSlicerZoom2DWidget::onCurrentSliceNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerZoom2DWidget);
  vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
  if (!sliceNode)
    return;

  if (!qvtkIsConnected(sliceNode, vtkCommand::ModifiedEvent,
                       this, SLOT(onSliceNodeModified(vtkObject*)))){
    qvtkConnect(sliceNode, vtkCommand::ModifiedEvent,
                this, SLOT(onSliceNodeModified(vtkObject*)));
  }

  setupWidgetFromZoom();
}

void qSlicerZoom2DWidget::onSliceGainChanged(double oldGain, double newGain)
{
  setupZoomFromWidget();
}

void qSlicerZoom2DWidget::onSliceNodeModified(vtkObject* sender)
{
  setupWidgetFromZoom();
}

void qSlicerZoom2DWidget::setupWidgetFromZoom()
{
  Q_D(qSlicerZoom2DWidget);
  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNodeSelector->currentNode());

  if (!sliceNode)
    return;

  if (!d->logic)
    return;

  double xyz[3];
  d->logic->GetSliceScale(sliceNode, xyz);

  d->xGainWidget->blockSignals(true);
  d->xGainWidget->setGain(xyz[0]);
  d->xGainWidget->blockSignals(false);

  d->yGainWidget->blockSignals(true);
  d->yGainWidget->setGain(xyz[1]);
  d->yGainWidget->blockSignals(false);
}

void qSlicerZoom2DWidget::setupZoomFromWidget()
{
  Q_D(qSlicerZoom2DWidget);
  vtkMRMLSliceNode* sliceNode =
      vtkMRMLSliceNode::SafeDownCast(d->sliceNodeSelector->currentNode());

  if (!sliceNode)
    return;

  double xGain, yGain,
      xFieldOfView, yFieldOfView, zFieldOfView;
  xGain = d->xGainWidget->getGain() > 0 ?
        d->xGainWidget->getGain() : 1;
  yGain = d->yGainWidget->getGain() > 0 ?
        d->yGainWidget->getGain() : 1;

  if (!d->logic)
    return;

  this->qvtkBlockAll();
  d->logic->ScaleSlice(sliceNode, xGain, yGain);
  this->qvtkUnblockAll();
}
