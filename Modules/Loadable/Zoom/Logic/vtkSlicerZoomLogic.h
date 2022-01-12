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

// .NAME vtkSlicerZoomLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerZoomLogic_h
#define __vtkSlicerZoomLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerZoomModuleLogicExport.h"

class vtkMRMLViewNode;
class vtkMRMLSliceNode;
class vtkCamera;
class vtkMRMLSliceLogic;
class vtkMRMLVolumeNode;
class qMRMLSliceWidget;
class qMRMLThreeDWidget;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_ZOOM_MODULE_LOGIC_EXPORT vtkSlicerZoomLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerZoomLogic *New();
  vtkTypeMacro(vtkSlicerZoomLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  void ScaleView(vtkMRMLViewNode* viewNode,
      double xScale, double yScale, double zScale);
  void ScaleSlice(vtkMRMLSliceNode* sliceNode,
      double hScale, double vScale);

  void ResizeSliceNode(
      vtkMRMLSliceNode* sliceNode, double newWidth, double newHeight);

  ///
  /// adjust the node's field of view to match the extent of all volume layers
  ///  (fits to first non-null layer)
  void FullFitSliceToAll(vtkMRMLSliceNode* sliceNode);

  void GetViewScale(
      vtkMRMLViewNode* viewNode, double xyz[3]);
  void GetSliceScale(
      vtkMRMLSliceNode* sliceNode, double xyz[3]);

  vtkMRMLSliceLogic* GetSliceLogic(vtkMRMLSliceNode* sliceNode);
  vtkCamera* GetCamera(vtkMRMLViewNode* viewNode);

  qMRMLSliceWidget* GetSliceWidget(vtkMRMLSliceNode* sliceNode);
  qMRMLThreeDWidget* GetThreeDWidget(vtkMRMLViewNode* viewNode);

protected:
  vtkSlicerZoomLogic();
  virtual ~vtkSlicerZoomLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:

  vtkSlicerZoomLogic(const vtkSlicerZoomLogic&); // Not implemented
  void operator=(const vtkSlicerZoomLogic&); // Not implemented
};

#endif
