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
#include <QToolButton>
#include <QHBoxLayout>

// Slicer includes
#include "qSlicerZoomModuleWidget.h"
#include "ui_qSlicerZoomModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// MRML includes
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSliceLogic.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDView.h"
#include "qMRMLLayoutViewFactory.h"
#include "qMRMLSliceControllerWidget.h"

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
  qSlicerApplication* app = nullptr;
};

//-----------------------------------------------------------------------------
// qSlicerZoomModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerZoomModuleWidgetPrivate::qSlicerZoomModuleWidgetPrivate(qSlicerZoomModuleWidget& object)
  : q_ptr()
{
  app = qSlicerApplication::application();
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

  // add button to already created slice widgets
  for (const auto& sliceName : d->app->layoutManager()->sliceViewNames()){
    qMRMLSliceWidget* sliceWidget =
        d->app->layoutManager()->sliceWidget(sliceName);

    if (!sliceWidget)
      continue;

    this->addFullFitToWindowToolButton(sliceWidget);
  }

  connect(this, &qSlicerZoomModuleWidget::mrmlSceneChanged,
          d->zoom3DWidget, &qSlicerZoom3DWidget::setMRMLScene);
  connect(this, &qSlicerZoomModuleWidget::mrmlSceneChanged,
          d->zoom2DWidget, &qSlicerZoom2DWidget::setMRMLScene);
}

void qSlicerZoomModuleWidget::addFullFitToWindowToolButton(
    qMRMLSliceWidget* sliceWidget)
{
  if (!sliceWidget ||
      !sliceWidget->sliceController() ||
      !sliceWidget->sliceController()->barWidget())
    return;

  qMRMLSliceControllerWidget* sliceController = sliceWidget->sliceController();
  QHBoxLayout* hLayout =
      qobject_cast<QHBoxLayout*>(sliceController->barLayout());

  if (!hLayout)
    return;

  QToolButton* fullFitToWindowBtn =
      sliceController->barWidget()->findChild<QToolButton*>("FullFitToWindowToolButton");

  if (fullFitToWindowBtn)
    return;

  fullFitToWindowBtn = new QToolButton(sliceController->barWidget());
  fullFitToWindowBtn->setObjectName("FullFitToWindowToolButton");
  fullFitToWindowBtn->setAutoRaise(true);
  fullFitToWindowBtn->setFixedSize(
        sliceController->fitToWindowToolButton()->size());
  fullFitToWindowBtn->setIcon(
        sliceController->fitToWindowToolButton()->icon());
  fullFitToWindowBtn->setToolTip("Adjust FOV without keeping aspect ratio");

  hLayout->insertWidget(3, fullFitToWindowBtn);

  connect(fullFitToWindowBtn, &QToolButton::clicked,
          this, &qSlicerZoomModuleWidget::onFullFitToWindowBtnClicked);
}

// `view` param is always `nullptr`
void qSlicerZoomModuleWidget::onSliceViewCreated(QWidget* view)
{
  Q_D(qSlicerZoomModuleWidget);
  if (!view)
    return;

  qMRMLSliceWidget* sliceWidget =
      dynamic_cast<qMRMLSliceWidget*>(view);

  this->addFullFitToWindowToolButton(sliceWidget);
}

void qSlicerZoomModuleWidget::onFullFitToWindowBtnClicked()
{
  Q_D(qSlicerZoomModuleWidget);
  vtkSlicerZoomLogic* zoomLogic =
      vtkSlicerZoomLogic::SafeDownCast(logic());

  if (!zoomLogic)
    return;

  QToolButton* fullFitToWindowBtn =
      qobject_cast<QToolButton*>(sender());

  if (!fullFitToWindowBtn)
    return;

  qMRMLSliceControllerWidget* sliceController =
      qobject_cast<qMRMLSliceControllerWidget*>(
        fullFitToWindowBtn->parent()->parent());

  if (!sliceController)
    return;

  // if slice is linked then apply FOV to all slice nodes
  if (sliceController->isLinked()){
    QStringList viewNames = d->app->layoutManager()->sliceViewNames();
    for (const auto& viewName : viewNames){
      zoomLogic->FullFitSliceToAll(
            d->app->layoutManager()->sliceWidget(viewName)->
            sliceController()->mrmlSliceNode());
    }
  } else {
    zoomLogic->FullFitSliceToAll(sliceController->mrmlSliceNode());
  }
}
