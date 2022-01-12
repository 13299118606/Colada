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

// SceneFilter Logic includes
#include <vtkSlicerSceneFilterLogic.h>

// SceneFilter includes
#include "qSlicerSceneFilterModule.h"
#include "qSlicerSceneFilterModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerSceneFilterModulePrivate
{
public:
  qSlicerSceneFilterModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerSceneFilterModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerSceneFilterModulePrivate::qSlicerSceneFilterModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerSceneFilterModule methods

//-----------------------------------------------------------------------------
qSlicerSceneFilterModule::qSlicerSceneFilterModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerSceneFilterModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerSceneFilterModule::~qSlicerSceneFilterModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerSceneFilterModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerSceneFilterModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSceneFilterModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerSceneFilterModule::icon() const
{
  return QIcon(":/Icons/SceneFilter.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerSceneFilterModule::categories() const
{
  return QStringList() << "Seismic";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSceneFilterModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
bool qSlicerSceneFilterModule::isHidden() const
{
  return true;
}

//-----------------------------------------------------------------------------
void qSlicerSceneFilterModule::setup()
{
  this->Superclass::setup();

  // need to be sure that the module widget was created
  this->widgetRepresentation();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerSceneFilterModule
::createWidgetRepresentation()
{
  return new qSlicerSceneFilterModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerSceneFilterModule::createLogic()
{
  return vtkSlicerSceneFilterLogic::New();
}
