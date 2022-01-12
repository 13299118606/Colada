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
#include "vtkSlicerZoomLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLCameraNode.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLVolumeNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCamera.h>
#include <vtkTransform.h>

// STD includes
#include <cassert>

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <vtkSlicerCamerasModuleLogic.h>
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceControllerWidget.h>

// Qt includes
#include <QToolButton>
#include <QHBoxLayout>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerZoomLogic);

//----------------------------------------------------------------------------
vtkSlicerZoomLogic::vtkSlicerZoomLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerZoomLogic::~vtkSlicerZoomLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerZoomLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerZoomLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerZoomLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerZoomLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerZoomLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
}

//---------------------------------------------------------------------------
void vtkSlicerZoomLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

void vtkSlicerZoomLogic::ScaleView(vtkMRMLViewNode* viewNode,
    double xScale, double yScale, double zScale)
{
  vtkCamera* camera = GetCamera(viewNode);

  if (!camera)
    return;

  double camPos[3], focalPoint[3];
  camera->GetPosition(camPos);
  camera->GetFocalPoint(focalPoint);

  vtkNew<vtkTransform> transform;
  transform->Scale(xScale, yScale, zScale);
  /* translation is needed to proportionally change view bounds when zooming.
   * It is difficult to explain why I use such expression for translation but
   * it was checked via Slicer's python interactor.
   * Camera position and focal point stay untouched */
  transform->Translate(
        focalPoint[0] * (1/xScale - 1),
      focalPoint[1] * (1/yScale - 1),
      focalPoint[2] * (1/zScale - 1));
  camera->SetModelTransformMatrix(transform->GetMatrix());
}

void vtkSlicerZoomLogic::ScaleSlice(vtkMRMLSliceNode* sliceNode,
    double hScale, double vScale)
{
  if (!sliceNode)
    return;

  int dims[3];  // dims[0] - width in pixels, dims[1] - height
  sliceNode->GetDimensions(dims);

  double xfov, yfov, zfov;
  sliceNode->GetFieldOfView(
        xfov, yfov, zfov);

  sliceNode->SetFieldOfView(
        double(dims[0])/hScale,
      double(dims[1])/vScale,
      zfov);
}

void vtkSlicerZoomLogic::ResizeSliceNode(
    vtkMRMLSliceNode* sliceNode, double newWidth, double newHeight)
{
  if (!sliceNode)
    {
    return;
    }

  // New size must be the active slice vtkRenderer size. It's the same than the window
  // if the layout is 1x1.
  newWidth /= sliceNode->GetLayoutGridColumns();
  newHeight /= sliceNode->GetLayoutGridRows();

  // The following was previously in SliceSWidget.tcl
//  double sliceStep = this->SliceSpacing[2];
  int oldDimensions[3];
  sliceNode->GetDimensions(oldDimensions);
  double oldFOV[3];
  sliceNode->GetFieldOfView(oldFOV);

  double scalingX = (newWidth != 0 && oldDimensions[0] != 0 ? newWidth / oldDimensions[0] : 1.);
  double scalingY = (newHeight != 0 && oldDimensions[1] != 0 ? newHeight / oldDimensions[1] : 1.);

  double magnitudeX = (scalingX >= 1. ? scalingX : 1. / scalingX);
  double magnitudeY = (scalingY >= 1. ? scalingY : 1. / scalingY);

  double newFOV[3];
  if (magnitudeX < magnitudeY)
    {
    newFOV[0] = oldFOV[0];
    newFOV[1] = oldFOV[1] * scalingY / scalingX;
    }
  else
    {
    newFOV[0] = oldFOV[0] * scalingX / scalingY;
    newFOV[1] = oldFOV[1];
    }
//  newFOV[2] = sliceStep * oldDimensions[2];
  double windowAspect = (newWidth != 0. ? newHeight / newWidth : 1.);
  double planeAspect = (newFOV[0] != 0. ? newFOV[1] / newFOV[0] : 1.);
  if (windowAspect != planeAspect)
    {
    newFOV[0] = (windowAspect != 0. ? newFOV[1] / windowAspect : newFOV[0]);
    }
  int disabled = sliceNode->StartModify();
  sliceNode->SetDimensions(newWidth, newHeight, oldDimensions[2]);
  sliceNode->SetFieldOfView(
        oldFOV[0]*newWidth/oldDimensions[0],
      oldFOV[1]*newHeight/oldDimensions[1],
      oldFOV[2]);
  sliceNode->EndModify(disabled);
}

void vtkSlicerZoomLogic::FullFitSliceToAll(vtkMRMLSliceNode* sliceNode)
{
  int width = -1;
  int height = -1;
  // Use SliceNode dimensions if width and height parameters are omitted
  if (width < 0 || height < 0)
  {
    int* dimensions = sliceNode->GetDimensions();
    width = dimensions ? dimensions[0] : -1;
    height = dimensions ? dimensions[1] : -1;
  }

  if (width < 0 || height < 0)
  {
    vtkErrorMacro(<< __FUNCTION__ << "- Invalid size:" << width
                  << "x" << height);
    return;
  }

  qMRMLSliceWidget* sliceWidget = GetSliceWidget(sliceNode);
  vtkMRMLVolumeNode *volumeNode;
  for ( int layer=0; layer < 3; layer++ )
  {
    volumeNode = sliceWidget->sliceLogic()->GetLayerVolumeNode (layer);
    if (volumeNode)
    {
      double rasDimensions[3], rasCenter[3];
      sliceWidget->sliceLogic()->GetVolumeRASBox(
            volumeNode, rasDimensions, rasCenter);
      double sliceDimensions[3], sliceCenter[3];
      sliceWidget->sliceLogic()->GetVolumeSliceDimensions(
            volumeNode, sliceDimensions, sliceCenter);

      double fitX, fitY, fitZ, displayX, displayY;
      displayX = fitX = fabs(sliceDimensions[0]);
      displayY = fitY = fabs(sliceDimensions[1]);
      fitZ = sliceWidget->sliceLogic()->GetVolumeSliceSpacing(
            volumeNode)[2] * sliceNode->GetDimensions()[2];

      double fov[3];
      sliceNode->GetFieldOfView(fov);

      sliceNode->SetFieldOfView(
            fitX,
          fitY,
          fitZ);

      //
      // set the origin to be the center of the volume in RAS
      //
      vtkNew<vtkMatrix4x4> sliceToRAS;
      sliceToRAS->DeepCopy(sliceNode->GetSliceToRAS());
      sliceToRAS->SetElement(0, 3, rasCenter[0]);
      sliceToRAS->SetElement(1, 3, rasCenter[1]);
      sliceToRAS->SetElement(2, 3, rasCenter[2]);
      sliceNode->GetSliceToRAS()->DeepCopy(sliceToRAS.GetPointer());
      sliceNode->SetSliceOrigin(0,0,0);
      //sliceNode->SetSliceOffset(offset);

      //TODO Fit UVW space
      sliceWidget->sliceLogic()->SnapSliceOffsetToIJK();
      sliceNode->UpdateMatrices();
      return;
    }
  }
}

void vtkSlicerZoomLogic::GetViewScale(
    vtkMRMLViewNode* viewNode, double xyz[3])
{
  vtkCamera* camera = GetCamera(viewNode);

  if (!camera)
    return;

  camera->GetModelViewTransformObject()->GetScale(xyz);
}

void vtkSlicerZoomLogic::GetSliceScale(
    vtkMRMLSliceNode* sliceNode, double xyz[3])
{
  if (!sliceNode)
    return;

  int dims[3];  // dims[0] - width in pixels, dims[1] - height
  sliceNode->GetDimensions(dims);

  double xfov, yfov, zfov;
  sliceNode->GetFieldOfView(
        xfov, yfov, zfov);

  xyz[0] = dims[0]/xfov;
  xyz[1] = dims[1]/yfov;
  xyz[2] = dims[2]/zfov;
}

vtkMRMLSliceLogic* vtkSlicerZoomLogic::GetSliceLogic(vtkMRMLSliceNode* sliceNode)
{
  if (!sliceNode)
    return nullptr;

  qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
  if (!layoutManager)
  {
    // application is closing
    return nullptr;
  }

  vtkCollection* sliceLogics = layoutManager->mrmlSliceLogics();
  vtkObject* object = nullptr;
  vtkCollectionSimpleIterator it;
  for (sliceLogics->InitTraversal(it); (object = sliceLogics->GetNextItemAsObject(it));)
  {
    vtkMRMLSliceLogic* sliceLogic = vtkMRMLSliceLogic::SafeDownCast(object);
    if (!sliceLogic)
    {
      continue;
    }

    if (sliceNode == sliceLogic->GetSliceNode())
      return sliceLogic;
  }

  return nullptr;
}

vtkCamera* vtkSlicerZoomLogic::GetCamera(vtkMRMLViewNode* viewNode)
{
  if (!viewNode)
    return nullptr;

  vtkSlicerCamerasModuleLogic* camerasLogic =
    vtkSlicerCamerasModuleLogic::SafeDownCast(this->GetModuleLogic("Cameras"));

  if (!camerasLogic)
    return nullptr;

  vtkMRMLCameraNode* cameraNode =
      camerasLogic->GetViewActiveCameraNode(viewNode);

  if (!cameraNode)
    return nullptr;

  return cameraNode->GetCamera();
}

qMRMLSliceWidget* vtkSlicerZoomLogic::GetSliceWidget(vtkMRMLSliceNode* sliceNode){
  qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
  if (!layoutManager)
  {
    // application is closing
    return nullptr;
  }

  return layoutManager->sliceWidget(sliceNode->GetName());
}

qMRMLThreeDWidget* vtkSlicerZoomLogic::GetThreeDWidget(vtkMRMLViewNode* viewNode)
{
  qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
  if (!layoutManager)
  {
    // application is closing
    return nullptr;
  }

  return layoutManager->threeDWidget(viewNode->GetName());
}
