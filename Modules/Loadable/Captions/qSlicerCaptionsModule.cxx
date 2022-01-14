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

// Captions Logic includes
#include <vtkSlicerCaptionsLogic.h>

// Captions includes
#include "qSlicerCaptionsModule.h"
#include "qSlicerCaptionsModuleWidget.h"

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerLayoutManager.h"

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
VTK_MODULE_INIT(vtkSlicerCaptionsModuleMRMLDisplayableManager)

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCaptionsModulePrivate
{
public:
  qSlicerCaptionsModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCaptionsModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCaptionsModulePrivate::qSlicerCaptionsModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCaptionsModule methods

//-----------------------------------------------------------------------------
qSlicerCaptionsModule::qSlicerCaptionsModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerCaptionsModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCaptionsModule::~qSlicerCaptionsModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCaptionsModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerCaptionsModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCaptionsModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCaptionsModule::icon() const
{
  return QIcon(":/Icons/Captions.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerCaptionsModule::categories() const
{
  return QStringList() << "Colada";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCaptionsModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerCaptionsModule::setup()
{
  this->Superclass::setup();

  // register DM factory
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->
      RegisterDisplayableManager("vtkMRMLCaptionsDisplayableManager");
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->
      RegisterDisplayableManager("vtkMRMLCaptionsDisplayableManager");

  // create axes buttons on the 3D views
  qSlicerCaptionsModuleWidget* moduleWidget = dynamic_cast<
      qSlicerCaptionsModuleWidget*>(this->widgetRepresentation());

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
          moduleWidget, &qSlicerCaptionsModuleWidget::onThreeDViewCreated);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCaptionsModule
::createWidgetRepresentation()
{
  return new qSlicerCaptionsModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCaptionsModule::createLogic()
{
  return vtkSlicerCaptionsLogic::New();
}
