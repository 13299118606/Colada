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
#include "qSlicerCaptionsModuleWidget.h"
#include "ui_qSlicerCaptionsModuleWidget.h"
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
class qSlicerCaptionsModuleWidgetPrivate: public Ui_qSlicerCaptionsModuleWidget
{
public:
  qSlicerCaptionsModuleWidgetPrivate();
  qSlicerApplication* app = nullptr;
};

//-----------------------------------------------------------------------------
// qSlicerCaptionsModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCaptionsModuleWidgetPrivate::qSlicerCaptionsModuleWidgetPrivate()
{
  app = qSlicerApplication::application();
}

//-----------------------------------------------------------------------------
// qSlicerCaptionsModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCaptionsModuleWidget::qSlicerCaptionsModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerCaptionsModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerCaptionsModuleWidget::~qSlicerCaptionsModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerCaptionsModuleWidget::setup()
{
  Q_D(qSlicerCaptionsModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // add button to already created 3D widgets
  for (int i = 0; i < d->app->layoutManager()->threeDViewCount(); i++){
    qMRMLThreeDWidget* threeDWidget =
        d->app->layoutManager()->threeDWidget(i);

    if (!threeDWidget)
      continue;

    this->addViewCaptionsToolButton(threeDWidget);
  }

  connect(this, &qSlicerCaptionsModuleWidget::mrmlSceneChanged,
          d->captionsWidget, &qSlicerCaptionsWidget::setMRMLScene);

  connect(d->captionsWidget, &qSlicerCaptionsWidget::captionsVisibilityToggled,
          this, &qSlicerCaptionsModuleWidget::onCaptionsVisibilityToggled);
}

void qSlicerCaptionsModuleWidget::addViewCaptionsToolButton(qMRMLThreeDWidget* threeDWidget)
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

  QToolButton* viewCaptionsBtn =
      popupWidget->findChild<QToolButton*>("CaptionsToolBtn");

  if (viewCaptionsBtn)
    return;

  viewCaptionsBtn = new QToolButton(popupWidget);
  viewCaptionsBtn->setObjectName("CaptionsToolBtn");
  viewCaptionsBtn->setToolTip("Show/hide captions");
  viewCaptionsBtn->setCheckable(true);
  viewCaptionsBtn->setChecked(true);

  for (int col = 0; col < gridLayout->columnCount(); col++){
    for (int row = 0; row < gridLayout->rowCount(); row++){
      if (row == gridLayout->rowCount()-1 &&
          col == gridLayout->columnCount()-1 &&
          gridLayout->itemAtPosition(row, col)){
        gridLayout->addWidget(viewCaptionsBtn, 0, gridLayout->columnCount());
        break;
      }

      if (!gridLayout->itemAtPosition(row, col)){
        gridLayout->addWidget(viewCaptionsBtn, row, col);
        break;
      }
    }
  }

  connect(viewCaptionsBtn, &QAbstractButton::toggled,
          this, &qSlicerCaptionsModuleWidget::onViewCaptionsBtnToggled);
}

void qSlicerCaptionsModuleWidget::onThreeDViewCreated(QWidget* view)
{
  if (!view)
    return;

  qMRMLThreeDWidget* threeDWidget =
      dynamic_cast<qMRMLThreeDWidget*>(view);

  this->addViewCaptionsToolButton(threeDWidget);
}


void qSlicerCaptionsModuleWidget::onViewCaptionsBtnToggled(bool checked)
{
  Q_D(qSlicerCaptionsModuleWidget);
  QToolButton* viewCaptionsBtn = qobject_cast<QToolButton*>(sender());
  if (!viewCaptionsBtn)
    return;

  ctkPopupWidget* popupWidget =
      qobject_cast<ctkPopupWidget*>(viewCaptionsBtn->parent());

  if (!popupWidget)
    return;

  qMRMLThreeDViewControllerWidget* controller =
      qobject_cast<qMRMLThreeDViewControllerWidget*>(popupWidget->parent());

  if (!controller)
    return;

  vtkMRMLViewNode* viewNode = controller->mrmlThreeDViewNode();
  if (!viewNode)
    return;

  d->captionsWidget->setCaptionsBtnToggled(checked);
}

void qSlicerCaptionsModuleWidget::onCaptionsVisibilityToggled(bool state, vtkMRMLNode* node)
{
  Q_D(qSlicerCaptionsModuleWidget);
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

    QToolButton* viewCaptionsBtn =
        popupWidget->findChild<QToolButton*>("CaptionsToolBtn");

    if (!viewCaptionsBtn)
      return;

    viewCaptionsBtn->setChecked(state);
    break;
  }
}
