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
#include "vtkSlicerAxesLogic.h"
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// CTK includes
#include "ctkPopupWidget.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLLayoutNode.h"

// MRMLQt includes
#include "qMRMLSliceWidget.h"
#include "qMRMLSliceView.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDViewControllerWidget.h"
#include "qMRMLLayoutManager.h"

// magic_enum includes
#include <magic_enum.hpp>

constexpr auto slicerLayoutValues = magic_enum::enum_values<vtkMRMLLayoutNode::SlicerLayout>();

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

  this->setupSliceAndViewNodes();

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

void qSlicerAxesModuleWidget::setupSliceAndViewNodes()
{
  Q_D(qSlicerAxesModuleWidget);
  vtkSlicerAxesLogic* axesLogic = vtkSlicerAxesLogic::SafeDownCast(logic());
  if(!axesLogic){
    qCritical() << Q_FUNC_INFO << ": unable to get Axes logic!";
    return;
  }

  if(!d->app->layoutManager()){
    qCritical() << Q_FUNC_INFO << ": unable to get Layout manager!";
    return;
  }

  if(!d->app->layoutManager()->mrmlScene()){
    qCritical() << Q_FUNC_INFO << ": invalid Scene!";
    return;
  }

  std::vector<std::string> axesNames({"-X", "X", "-Y", "Y", "-Z", "Z"});
  std::vector<std::string> orientationPresetOld({"Axial", "Sagittal", "Coronal"});
  std::vector<std::string> orientationPresetNew({"XY", "YZ", "XZ"});
  std::vector<std::string> defaultOrientations({"XY", "YZ", "XZ"});

  // Default View nodes
  vtkSmartPointer<vtkMRMLNode> dViewNode =
      d->app->layoutManager()->mrmlScene()->GetDefaultNodeByClass("vtkMRMLViewNode");
  if (!dViewNode)
  {
    vtkMRMLNode *node = d->app->layoutManager()->mrmlScene()->CreateNodeByClass("vtkMRMLViewNode");
    dViewNode.TakeReference(node);
    d->app->layoutManager()->mrmlScene()->AddDefaultNode(dViewNode);
  }
  axesLogic->OverwriteAxesLabelsAndPresetsToXYZ(dViewNode);

  // Default Slice nodes
  vtkSmartPointer<vtkMRMLNode> dSliceNode =
      d->app->layoutManager()->mrmlScene()->GetDefaultNodeByClass("vtkMRMLSliceNode");
  if (!dSliceNode)
  {
    vtkMRMLNode *node = d->app->layoutManager()->mrmlScene()->CreateNodeByClass("vtkMRMLSliceNode");
    dSliceNode.TakeReference(node);
    d->app->layoutManager()->mrmlScene()->AddDefaultNode(dSliceNode);
  }
  axesLogic->OverwriteAxesLabelsAndPresetsToXYZ(dSliceNode, defaultOrientations[0]);

  // View nodes
  for (int j = 0; j < d->app->layoutManager()->threeDViewCount(); j++)
    axesLogic->OverwriteAxesLabelsAndPresetsToXYZ(
          d->app->layoutManager()->threeDWidget(j)->mrmlViewNode());

  // Slice nodes
  size_t ii = 0;
  for (const QString& sliceViewName : d->app->layoutManager()->sliceViewNames()){
    vtkMRMLSliceNode *sliceNode = d->app->layoutManager()->
        sliceWidget(sliceViewName)->mrmlSliceNode();
    std::string defaultOrientation = ii < defaultOrientations.size() ?
          defaultOrientations[ii] : defaultOrientations[0];
    axesLogic->OverwriteAxesLabelsAndPresetsToXYZ(sliceNode, defaultOrientation);
    ii++;
  }

  // Replace layout description with new presets
  vtkMRMLLayoutNode* layoutNode =  vtkMRMLLayoutNode::SafeDownCast(
    d->app->layoutManager()->mrmlScene()->GetSingletonNode("vtkMRMLLayoutNode","vtkMRMLLayoutNode"));
  if(!layoutNode){
    qCritical() << Q_FUNC_INFO << ": layoutNode not found!";
    return;
  }

  for (const auto& i : slicerLayoutValues){
    std::string layoutDescription = layoutNode->GetLayoutDescription(i);
    std::vector<std::string>::const_iterator it;
    std::vector<std::string>::const_iterator jt;
    for(it = orientationPresetOld.begin(), jt = orientationPresetNew.begin();
        it != orientationPresetOld.end() && jt != orientationPresetNew.end(); ++it, ++jt)
    {
      size_t found = layoutDescription.find(*it);
      while (found!=std::string::npos)
      {
        layoutDescription.replace(found, it->size(), *jt);
        found = layoutDescription.find(*it);
      }
    }
    layoutNode->SetLayoutDescription(i, layoutDescription.c_str());
  }
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
