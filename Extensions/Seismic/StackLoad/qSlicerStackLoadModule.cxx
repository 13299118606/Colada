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

// StackLoad Logic includes
#include <vtkSlicerStackLoadLogic.h>

// StackLoad includes
#include "qSlicerStackLoadModule.h"
#include "qSlicerStackLoadModuleWidget.h"

// Colada includes
#include "qColadaAppMainWindow.h"
#include "qColadaH5SeisTreeView.h"
#include "qColadaH5SeisModel.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Item.h"

// SLicer includes
#include "qSlicerApplication.h"
#include "qSlicerModuleSelectorToolBar.h"
#include "qSlicerModulesMenu.h"
#include "qSlicerLayoutManager.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLThreeDView.h"
#include "qMRMLThreeDViewControllerWidget.h"

// Qt includes
#include <QToolButton>
#include <QGridLayout>

// ctk includes
#include <ctkPopupWidget.h>

// h5geo includes
#include <h5geo/h5core.h>
#include <h5geo/h5seis.h>
#include <h5geo/h5seiscontainer.h>

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerStackLoadModulePrivate
{
public:
  qSlicerStackLoadModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerStackLoadModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerStackLoadModulePrivate::qSlicerStackLoadModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerStackLoadModule methods

//-----------------------------------------------------------------------------
qSlicerStackLoadModule::qSlicerStackLoadModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerStackLoadModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerStackLoadModule::~qSlicerStackLoadModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerStackLoadModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerStackLoadModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerStackLoadModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerStackLoadModule::icon() const
{
  return QIcon(":/Icons/StackLoad.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerStackLoadModule::categories() const
{
  return QStringList() << "Seismic";
}

//-----------------------------------------------------------------------------
QStringList qSlicerStackLoadModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerStackLoadModule::setup()
{
  this->Superclass::setup();

  qSlicerApplication* app = qSlicerApplication::application();
  auto* mainW = qobject_cast<qColadaAppMainWindow*>(app->mainWindow());
  auto* mainWSeisTreeView = qobject_cast<qColadaH5SeisTreeView*>(mainW->getSeisTreeView());
  auto* mainWSeisProxy = qobject_cast<qColadaH5ProxyModel*>(mainWSeisTreeView->model());
  auto* mainWSeisModel = qobject_cast<qColadaH5SeisModel*>(mainWSeisProxy->sourceModel());

  connect(
        mainWSeisModel, &QAbstractItemModel::dataChanged,
        this, &qSlicerStackLoadModule::onSeisModelDataChanged);

  this->setupUI();
}

void qSlicerStackLoadModule::setupUI()
{
  qSlicerApplication* app = qSlicerApplication::application();
  qMRMLThreeDViewControllerWidget* controller = app->layoutManager()->
      threeDWidget(0)->threeDController();

  ctkPopupWidget* popupWidget = controller->
      findChild<ctkPopupWidget*>("qMRMLThreeDViewControllerWidget");

  QGridLayout* gridLayout = popupWidget->findChild<QGridLayout*>("gridLayout");

  QToolButton* cubeAxesToolBtn = new QToolButton(popupWidget);
  cubeAxesToolBtn->setObjectName("cubeAxesToolBtn");
  cubeAxesToolBtn->setToolTip("Show/hide cube axes actor");
  cubeAxesToolBtn->setCheckable(true);

  gridLayout->addWidget(cubeAxesToolBtn, 0, 4);

  connect(cubeAxesToolBtn, &QAbstractButton::toggled,
          this, &qSlicerStackLoadModule::onCubeAxesToolBtnToggled);
}

void qSlicerStackLoadModule::setupUILogic()
{

}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerStackLoadModule
::createWidgetRepresentation()
{
  return new qSlicerStackLoadModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerStackLoadModule::createLogic()
{
  return vtkSlicerStackLoadLogic::New();
}

void qSlicerStackLoadModule::onSeisModelDataChanged(
    const QModelIndex &topLeft,
    const QModelIndex &bottomRight,
    const QVector<int> &roles)
{
  if (!topLeft.isValid() ||
      !bottomRight.isValid() ||
      roles.empty())
    return;

  qSlicerStackLoadModuleWidget* moduleWidget = dynamic_cast<
      qSlicerStackLoadModuleWidget*>(this->widgetRepresentation());

  if (!moduleWidget)
    return;

  qSlicerApplication* app = qSlicerApplication::application();
  auto* mainW = qobject_cast<qColadaAppMainWindow*>(app->mainWindow());
  auto* mainWSeisTreeView = qobject_cast<qColadaH5SeisTreeView*>(mainW->getSeisTreeView());
  auto* mainWSeisProxy = qobject_cast<qColadaH5ProxyModel*>(mainWSeisTreeView->model());
  auto* mainWSeisModel = qobject_cast<qColadaH5SeisModel*>(mainWSeisProxy->sourceModel());

  int n = 0;
  for (int row = topLeft.row(); row <= bottomRight.row(); row++){
    for (int col = topLeft.column(); col <= bottomRight.column(); col++){
      if (n >= roles.count())
        return;

      if (roles[n] != Qt::CheckStateRole)
        continue;

      if (mainW->moduleSelector()->selectedModule().
          compare(moduleWidget->moduleName(), Qt::CaseInsensitive))
        mainW->moduleSelector()->selectModule(moduleWidget->moduleName());

      qColadaH5Item* item = mainWSeisModel->itemFromIndex(
            mainWSeisModel->index(row, col, topLeft.parent()));
      H5Seis* seis = dynamic_cast<H5Seis*>(item->getGeoObject());
      h5gt::Group objG = seis->getObjG();
      if (!h5geo::isSeis(objG))
        continue;

      if (item->checkState() == Qt::Checked)
        moduleWidget->addSeis(seis);
      else if (item->checkState() == Qt::Unchecked)
        moduleWidget->removeSeis(seis);

      n++;
    }
  }
}

void qSlicerStackLoadModule::onCubeAxesToolBtnToggled(bool val)
{
//  qSlicerApplication* app = qSlicerApplication::application();
//  vtkSlicerCubeAxesDisplayableManager3d* cubeAxesManager =
//      dynamic_cast<vtkSlicerCubeAxesDisplayableManager3d*>(
//        app->layoutManager()->threeDWidget(0)->threeDView()->
//        displayableManagerByClassName("vtkSlicerCubeAxesDisplayableManager3d"));

//  cubeAxesManager->showAxes(val);
}
