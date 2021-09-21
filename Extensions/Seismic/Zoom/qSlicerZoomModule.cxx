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

// Zoom Logic includes
#include <vtkSlicerZoomLogic.h>

// Zoom includes
#include "qSlicerZoomModule.h"
#include "qSlicerZoomModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerZoomModulePrivate
{
public:
  qSlicerZoomModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerZoomModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerZoomModulePrivate::qSlicerZoomModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerZoomModule methods

//-----------------------------------------------------------------------------
qSlicerZoomModule::qSlicerZoomModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerZoomModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerZoomModule::~qSlicerZoomModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerZoomModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerZoomModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerZoomModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerZoomModule::icon() const
{
  return QIcon(":/Icons/Zoom.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerZoomModule::categories() const
{
  return QStringList() << "Seismic";
}

//-----------------------------------------------------------------------------
QStringList qSlicerZoomModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerZoomModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerZoomModule
::createWidgetRepresentation()
{
  return new qSlicerZoomModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerZoomModule::createLogic()
{
  return vtkSlicerZoomLogic::New();
}
