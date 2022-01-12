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

// .NAME vtkSlicerAxesLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerAxesLogic_h
#define __vtkSlicerAxesLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerAxesModuleLogicExport.h"

template<class T>
class vtkSmartPointer;
class vtkMatrix3x3;


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_AXES_MODULE_LOGIC_EXPORT vtkSlicerAxesLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerAxesLogic *New();
  vtkTypeMacro(vtkSlicerAxesLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkSlicerAxesLogic();
  virtual ~vtkSlicerAxesLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  vtkSmartPointer<vtkMatrix3x3> GenerateOrientationMatrix(const std::string& name);
  vtkSmartPointer<vtkMatrix3x3> GenerateXYOrientationMatrix();
  vtkSmartPointer<vtkMatrix3x3> GenerateXZOrientationMatrix();
  vtkSmartPointer<vtkMatrix3x3> GenerateYZOrientationMatrix();
private:

  vtkSlicerAxesLogic(const vtkSlicerAxesLogic&); // Not implemented
  void operator=(const vtkSlicerAxesLogic&); // Not implemented
};

#endif
