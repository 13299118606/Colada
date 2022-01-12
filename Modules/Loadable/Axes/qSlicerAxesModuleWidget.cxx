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

// Slicer includes
#include "qSlicerAxesModuleWidget.h"
#include "ui_qSlicerAxesModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// CTK includes
#include "ctkPopupWidget.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLViewNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDViewControllerWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerAxesModuleWidgetPrivate: public Ui_qSlicerAxesModuleWidget
{
public:
  qSlicerAxesModuleWidgetPrivate();
  qSlicerApplication* app = nullptr;
};

//-----------------------------------------------------------------------------
// qSlicerAxesModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerAxesModuleWidgetPrivate::qSlicerAxesModuleWidgetPrivate()
{
  app = qSlicerApplication::application();
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

  // add button to already created 3D widgets
  for (int i = 0; i < d->app->layoutManager()->threeDViewCount(); i++){
    qMRMLThreeDWidget* threeDWidget =
        d->app->layoutManager()->threeDWidget(i);

    if (!threeDWidget)
      continue;

    this->addViewAxesToolButton(threeDWidget);
  }

  connect(this, &qSlicerAxesModuleWidget::mrmlSceneChanged,
          d->axes3DWidget, &qSlicerAxes3DWidget::setMRMLScene);

  connect(d->axes3DWidget, &qSlicerAxes3DWidget::axes3DVisibilityToggled,
          this, &qSlicerAxesModuleWidget::onAxes3DVisibilityToggled);
}

void qSlicerAxesModuleWidget::addViewAxesToolButton(qMRMLThreeDWidget* threeDWidget)
{
  if (!threeDWidget ||
      !threeDWidget->threeDController())
    return;

  qMRMLThreeDViewControllerWidget* controller = threeDWidget->threeDController();
  ctkPopupWidget* popupWidget = controller->
      findChild<ctkPopupWidget*>("qMRMLThreeDViewControllerWidget");

  if (!popupWidget)
    return;

  QGridLayout* gridLayout = popupWidget->findChild<QGridLayout*>("gridLayout");

  if (!gridLayout)
    return;

  QToolButton* viewAxesBtn =
      popupWidget->findChild<QToolButton*>("CubeAxesToolBtn");

  if (viewAxesBtn)
    return;

  viewAxesBtn = new QToolButton(popupWidget);
  viewAxesBtn->setObjectName("CubeAxesToolBtn");
  viewAxesBtn->setToolTip("Show/hide cube axes actor");
  viewAxesBtn->setCheckable(true);

  for (int col = 0; col < gridLayout->columnCount(); col++){
    for (int row = 0; row < gridLayout->rowCount(); row++){
      if (row == gridLayout->rowCount()-1 &&
          col == gridLayout->columnCount()-1 &&
          gridLayout->itemAtPosition(row, col)){
        gridLayout->addWidget(viewAxesBtn, 0, gridLayout->columnCount());
        break;
      }

      if (!gridLayout->itemAtPosition(row, col)){
        gridLayout->addWidget(viewAxesBtn, row, col);
        break;
      }
    }
  }

  connect(viewAxesBtn, &QAbstractButton::toggled,
          this, &qSlicerAxesModuleWidget::onViewAxesBtnToggled);
}

void qSlicerAxesModuleWidget::onThreeDViewCreated(QWidget* view)
{
  if (!view)
    return;

  qMRMLThreeDWidget* threeDWidget =
      dynamic_cast<qMRMLThreeDWidget*>(view);

  this->addViewAxesToolButton(threeDWidget);
}

void qSlicerAxesModuleWidget::onViewAxesBtnToggled(bool checked)
{
  Q_D(qSlicerAxesModuleWidget);
  QToolButton* viewAxesBtn = qobject_cast<QToolButton*>(sender());
  if (!viewAxesBtn)
    return;

  ctkPopupWidget* popupWidget =
      qobject_cast<ctkPopupWidget*>(viewAxesBtn->parent());

  if (!popupWidget)
    return;

  qMRMLThreeDViewControllerWidget* controller =
      qobject_cast<qMRMLThreeDViewControllerWidget*>(popupWidget->parent());

  if (!controller)
    return;

  vtkMRMLViewNode* viewNode = controller->mrmlThreeDViewNode();
  if (!viewNode)
    return;

  d->axes3DWidget->onAxesBtnToggled(checked);
}

void qSlicerAxesModuleWidget::onAxes3DVisibilityToggled(bool state, vtkMRMLNode* node)
{
  Q_D(qSlicerAxesModuleWidget);
  vtkMRMLViewNode* viewNode = dynamic_cast<vtkMRMLViewNode*>(node);
  if (!viewNode)
    return;

  for (int i = 0; i < d->app->layoutManager()->threeDViewCount(); i++){
    qMRMLThreeDWidget* threeDWidget =
        d->app->layoutManager()->threeDWidget(i);

    if (!threeDWidget ||
        !threeDWidget->threeDController() ||
        threeDWidget->mrmlViewNode() != viewNode)
      continue;

    qMRMLThreeDViewControllerWidget* controller = threeDWidget->threeDController();
    ctkPopupWidget* popupWidget = controller->
        findChild<ctkPopupWidget*>("qMRMLThreeDViewControllerWidget");

    if (!popupWidget)
      return;

    QToolButton* viewAxesBtn =
        popupWidget->findChild<QToolButton*>("CubeAxesToolBtn");

    if (!viewAxesBtn)
      return;

    viewAxesBtn->setChecked(state);
    break;
  }
}
