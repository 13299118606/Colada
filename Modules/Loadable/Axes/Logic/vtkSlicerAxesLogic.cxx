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
#include "vtkSlicerAxesLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSliceNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix3x3.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerAxesLogic);

//----------------------------------------------------------------------------
vtkSlicerAxesLogic::vtkSlicerAxesLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerAxesLogic::~vtkSlicerAxesLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerAxesLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerAxesLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerAxesLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerAxesLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerAxesLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  this->OverwriteAxesLabelsAndPresetsToXYZ(node);
}

void vtkSlicerAxesLogic
::OverwriteAxesLabelsAndPresetsToXYZ(
    vtkMRMLNode* node, const std::string& defaultOrientation)
{
  if (!node)
    return;

  std::vector<std::string> axesNames({"-X", "X", "-Y", "Y", "-Z", "Z"});
  std::vector<std::string> orientationPresetOld({"Axial", "Sagittal", "Coronal"});
  std::vector<std::string> orientationPresetNew({"XY", "YZ", "XZ"});

  //change axes label names
  if (node->IsA("vtkMRMLViewNode")){
    vtkMRMLViewNode *viewNode =
        vtkMRMLViewNode::SafeDownCast(node);

    viewNode->DisableModifiedEventOn();
    for (size_t i = 0; i < axesNames.size(); i++)
      viewNode->SetAxisLabel(i, axesNames[i].c_str());
    viewNode->DisableModifiedEventOff();
  }

  if (node->IsA("vtkMRMLSliceNode")){
    vtkMRMLSliceNode *sliceNode =
        vtkMRMLSliceNode::SafeDownCast(node);

    sliceNode->DisableModifiedEventOn();
    for (size_t i = 0; i < axesNames.size(); i++)
      sliceNode->SetAxisLabel(i, axesNames[i].c_str());

    for (size_t i = 0; i < orientationPresetOld.size(); i++){
      if (sliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
        sliceNode->RemoveSliceOrientationPreset(
              orientationPresetOld[i]);
      }

      if (!sliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
        vtkSmartPointer<vtkMatrix3x3> mtx = GenerateOrientationMatrix(orientationPresetNew[i]);
        if (!mtx)
          continue;

        sliceNode->AddSliceOrientationPreset(
              orientationPresetNew[i], mtx);
      }
    }
    if (!defaultOrientation.empty())
      sliceNode->SetDefaultOrientation(defaultOrientation.c_str());
    sliceNode->DisableModifiedEventOff();
  }
}

//---------------------------------------------------------------------------
void vtkSlicerAxesLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

vtkSmartPointer<vtkMatrix3x3> vtkSlicerAxesLogic::GenerateOrientationMatrix(const std::string& name)
{
  if (name.compare("XY") == 0 || name.compare("YX") == 0)
    return GenerateXYOrientationMatrix();
  else if (name.compare("XZ") == 0 || name.compare("ZX") == 0)
    return GenerateXZOrientationMatrix();
  else if (name.compare("YZ") == 0 || name.compare("ZY") == 0)
    return GenerateYZOrientationMatrix();
  else
    return vtkSmartPointer<vtkMatrix3x3>::New();
}

vtkSmartPointer<vtkMatrix3x3> vtkSlicerAxesLogic::GenerateXYOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  return mtx;
}

vtkSmartPointer<vtkMatrix3x3> vtkSlicerAxesLogic::GenerateXZOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  mtx->SetElement(0,0,1);
  mtx->SetElement(1,1,0);
  mtx->SetElement(2,2,0);
  mtx->SetElement(1,2,-1);
  mtx->SetElement(2,1,1);
  return mtx;
}

vtkSmartPointer<vtkMatrix3x3> vtkSlicerAxesLogic::GenerateYZOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  mtx->SetElement(0,0,0);
  mtx->SetElement(1,1,0);
  mtx->SetElement(2,2,0);
  mtx->SetElement(0,2,-1);
  mtx->SetElement(1,0,1);
  mtx->SetElement(2,1,1);
  return mtx;
}
