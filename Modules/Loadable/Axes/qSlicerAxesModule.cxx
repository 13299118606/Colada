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

// Axes Logic includes
#include <vtkSlicerAxesLogic.h>

// Axes includes
#include "qSlicerAxesModule.h"
#include "qSlicerAxesModuleWidget.h"

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

// VTK includes
#include <vtkSmartPointer.h>

// MRML includes
#include "vtkMRMLSliceViewDisplayableManagerFactory.h"
#include "vtkMRMLThreeDViewDisplayableManagerFactory.h"
#include "vtkMRMLAxesDisplayableManager2D.h"
#include "vtkMRMLAxesDisplayableManager3D.h"
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLScene.h>

// qMRMLIncludes
#include "qMRMLLayoutManager.h"
#include "qMRMLLayoutManager_p.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkSlicerAxesModuleMRMLDisplayableManager)

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerAxesModulePrivate
{
public:
  qSlicerAxesModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerAxesModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerAxesModulePrivate::qSlicerAxesModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerAxesModule methods

//-----------------------------------------------------------------------------
qSlicerAxesModule::qSlicerAxesModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerAxesModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerAxesModule::~qSlicerAxesModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerAxesModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerAxesModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerAxesModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerAxesModule::icon() const
{
  return QIcon(":/Icons/Axes.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerAxesModule::categories() const
{
  return QStringList() << "Colada";
}

//-----------------------------------------------------------------------------
QStringList qSlicerAxesModule::dependencies() const
{
  return QStringList() << "DataProbe";
}

//-----------------------------------------------------------------------------
void qSlicerAxesModule::setup()
{
  this->Superclass::setup();

  // register DM factory
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->
      RegisterDisplayableManager("vtkMRMLAxesDisplayableManager2D");
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->
      RegisterDisplayableManager("vtkMRMLAxesDisplayableManager3D");

  // create axes buttons on the 3D views
  qSlicerAxesModuleWidget* moduleWidget = dynamic_cast<
      qSlicerAxesModuleWidget*>(this->widgetRepresentation());

  if (!moduleWidget)
    return;

  qSlicerApplication* app = qSlicerApplication::application();

  qMRMLLayoutViewFactory* viewFactory = app->
        layoutManager()->mrmlViewFactory("vtkMRMLViewNode");

  qMRMLLayoutThreeDViewFactory* threeDViewFactory =
      dynamic_cast<qMRMLLayoutThreeDViewFactory*>(viewFactory);

  if (!threeDViewFactory)
    return;

  connect(threeDViewFactory, &qMRMLLayoutSliceViewFactory::viewCreated,
          moduleWidget, &qSlicerAxesModuleWidget::onThreeDViewCreated);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerAxesModule
::createWidgetRepresentation()
{
  return new qSlicerAxesModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerAxesModule::createLogic()
{
  return vtkSlicerAxesLogic::New();
}
