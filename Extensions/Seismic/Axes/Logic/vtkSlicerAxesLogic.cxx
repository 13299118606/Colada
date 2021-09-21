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
  if (!node || !this->GetMRMLScene())
  {
    return;
  }

  std::vector<std::string> axesNames({"-X", "X", "-Y", "Y", "-Z", "Z"});
  std::vector<std::string> orientationPresetOld({"Axial", "Sagittal", "Coronal"});
  std::vector<std::string> orientationPresetNew({"XY", "YZ", "XZ"});
  std::vector<std::string> defaultOrientation({"XY", "YZ", "XZ"});

  //change axes label names
  if (node->IsA("vtkMRMLViewNode"))
  {
    vtkMRMLViewNode *viewNode =
        vtkMRMLViewNode::SafeDownCast(node);

    viewNode->DisableModifiedEventOn();

    for (size_t i = 0; i < axesNames.size(); i++)
      viewNode->SetAxisLabel(
            i, axesNames[i].c_str());

    viewNode->DisableModifiedEventOff();
  }

  if (node->IsA("vtkMRMLSliceNode"))
  {
    vtkMRMLSliceNode *sliceNode =
        vtkMRMLSliceNode::SafeDownCast(node);

    sliceNode->DisableModifiedEventOn();

    for (size_t i = 0; i < axesNames.size(); i++)
      sliceNode->SetAxisLabel(
            i, axesNames[i].c_str());

    for (size_t i = 0; i < orientationPresetOld.size(); i++){
      if (sliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
        sliceNode->RemoveSliceOrientationPreset(
              orientationPresetOld[i]);
      }

      if (!sliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
        sliceNode->AddSliceOrientationPreset(
              orientationPresetNew[i],
              GenerateOrientationMatrix(orientationPresetNew[i]));
        sliceNode->SetDefaultOrientation(defaultOrientation[i].c_str());
      }
    }

//    if (std::strcmp(sliceNode->GetName(), "Red") == 0)
//      sliceNode->SetDefaultOrientation(defaultOrientation[0].c_str());
//    else if (std::strcmp(sliceNode->GetName(), "Green") == 0)
//      sliceNode->SetDefaultOrientation(defaultOrientation[1].c_str());
//    else if (std::strcmp(sliceNode->GetName(), "Yellow") == 0)
//      sliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());
//    else
//      sliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());

    sliceNode->DisableModifiedEventOff();
  }

  // Modify SliceNodes already allocated
  vtkSmartPointer<vtkCollection> viewNodes = vtkSmartPointer<vtkCollection>::Take
      (this->GetMRMLScene()->GetNodesByClass("vtkMRMLViewNode"));

  for(int i = 0; i < viewNodes->GetNumberOfItems(); i++)
  {
    vtkMRMLViewNode* viewNode =
        vtkMRMLViewNode::SafeDownCast(viewNodes->GetItemAsObject(i));
    if (viewNode)
    {
      viewNode->DisableModifiedEventOn();

      for (size_t i = 0; i < axesNames.size(); i++)
        viewNode->SetAxisLabel(
              i, axesNames[i].c_str());

      viewNode->DisableModifiedEventOff();
    }
  }

  // Set Slice Default Node
  vtkSmartPointer<vtkMRMLNode> defaultNode = vtkMRMLSliceNode::SafeDownCast
      (this->GetMRMLScene()->GetDefaultNodeByClass("vtkMRMLSliceNode"));
  if (!defaultNode)
  {
    vtkMRMLNode *foo = this->GetMRMLScene()->CreateNodeByClass("vtkMRMLSliceNode");
    defaultNode.TakeReference(foo);
    this->GetMRMLScene()->AddDefaultNode(defaultNode);
  }
  vtkMRMLSliceNode *defaultSliceNode = vtkMRMLSliceNode::SafeDownCast(defaultNode);
  defaultSliceNode->DisableModifiedEventOn();

  for (size_t i = 0; i < axesNames.size(); i++)
    defaultSliceNode->SetAxisLabel(
          i, axesNames[i].c_str());

  for (size_t i = 0; i < orientationPresetOld.size(); i++){
    if (defaultSliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
      defaultSliceNode->RemoveSliceOrientationPreset(
            orientationPresetOld[i]);
    }

    if (!defaultSliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
      defaultSliceNode->AddSliceOrientationPreset(
            orientationPresetNew[i],
            GenerateOrientationMatrix(orientationPresetNew[i]));
      defaultSliceNode->SetDefaultOrientation(defaultOrientation[i].c_str());
    }
  }

//  if (std::strcmp(defaultSliceNode->GetName(), "Red") == 0)
//    defaultSliceNode->SetDefaultOrientation(defaultOrientation[0].c_str());
//  else if (std::strcmp(defaultSliceNode->GetName(), "Green") == 0)
//    defaultSliceNode->SetDefaultOrientation(defaultOrientation[1].c_str());
//  else if (std::strcmp(defaultSliceNode->GetName(), "Yellow") == 0)
//    defaultSliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());
//  else
//    defaultSliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());

  defaultSliceNode->DisableModifiedEventOff();

  // Modify SliceNodes already allocated
  vtkSmartPointer<vtkCollection> sliceNodes = vtkSmartPointer<vtkCollection>::Take
      (this->GetMRMLScene()->GetNodesByClass("vtkMRMLSliceNode"));

  for(int j = 0; j < sliceNodes->GetNumberOfItems(); j++)
  {
    vtkMRMLSliceNode* sliceNode =
        vtkMRMLSliceNode::SafeDownCast(sliceNodes->GetItemAsObject(j));
    if (sliceNode)
    {
      sliceNode->DisableModifiedEventOn();

      for (size_t i = 0; i < axesNames.size(); i++)
        sliceNode->SetAxisLabel(
              i, axesNames[i].c_str());

      for (size_t i = 0; i < orientationPresetOld.size(); i++){
        if (sliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
          sliceNode->RemoveSliceOrientationPreset(
                orientationPresetOld[i]);
        }

        if (!sliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
          sliceNode->AddSliceOrientationPreset(
                orientationPresetNew[i],
                GenerateOrientationMatrix(orientationPresetNew[i]));
          sliceNode->SetDefaultOrientation(defaultOrientation[i].c_str());
        }
      }

//      if (std::strcmp(sliceNode->GetName(), "Red") == 0)
//        sliceNode->SetDefaultOrientation(defaultOrientation[0].c_str());
//      else if (std::strcmp(sliceNode->GetName(), "Green") == 0)
//        sliceNode->SetDefaultOrientation(defaultOrientation[1].c_str());
//      else if (std::strcmp(sliceNode->GetName(), "Yellow") == 0)
//        sliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());
//      else
//        sliceNode->SetDefaultOrientation(defaultOrientation[2].c_str());

      sliceNode->DisableModifiedEventOff();
    }
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
