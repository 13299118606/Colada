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

// SpatialReference includes
#include "qSlicerSpatialReferenceModule.h"
#include "qSlicerSpatialReferenceSettingsPanel.h"

// Slicer includes
#include "qSlicerApplication.h"

// Qt inludes
#include <QDebug>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerSpatialReferenceModulePrivate
{
public:
  qSlicerSpatialReferenceModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerSpatialReferenceModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerSpatialReferenceModulePrivate::qSlicerSpatialReferenceModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerSpatialReferenceModule methods

//-----------------------------------------------------------------------------
qSlicerSpatialReferenceModule::qSlicerSpatialReferenceModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerSpatialReferenceModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerSpatialReferenceModule::~qSlicerSpatialReferenceModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerSpatialReferenceModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerSpatialReferenceModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSpatialReferenceModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerSpatialReferenceModule::icon() const
{
  return QIcon(":/Icons/SpatialReference.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerSpatialReferenceModule::categories() const
{
  return QStringList() << "Colada";
}

//-----------------------------------------------------------------------------
QStringList qSlicerSpatialReferenceModule::dependencies() const
{
  return QStringList() << "Units";
}

//-----------------------------------------------------------------------------
bool qSlicerSpatialReferenceModule::isHidden() const
{
  return true;
}

//-----------------------------------------------------------------------------
void qSlicerSpatialReferenceModule::setup()
{
  this->Superclass::setup();

  qSlicerApplication* app = qSlicerApplication::application();
  if (!app)
    qCritical() << "qSlicerSpatialReferenceModule::setup: unable to get instance of application";
  if (app)
  {
    qSlicerSpatialReferenceSettingsPanel* panel = new qSlicerSpatialReferenceSettingsPanel;
    app->settingsDialog()->addPanel(
          qSlicerApplication::tr("Spatial reference"), panel);
  }
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerSpatialReferenceModule
::createWidgetRepresentation()
{
  return nullptr;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerSpatialReferenceModule::createLogic()
{
  return nullptr;
}
