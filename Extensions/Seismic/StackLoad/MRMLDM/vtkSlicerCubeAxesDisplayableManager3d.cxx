// MRMLDisplayableManager includes
#include "vtkSlicerCubeAxesDisplayableManager3d.h"

// MRML includes
#include <vtkMRMLLogic.h>
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkMRMLInteractionNode.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLVolumeRenderingDisplayableManager.h"

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkObjectFactory.h>
#include <vtkCubeAxesActor.h>
#include <vtkRenderer.h>

// STD includes
#include <sstream>

// SlicerQt includes
#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>

// vtkSlicer includes
#include <vtkSlicerApplicationLogic.h>

vtkStandardNewMacro(vtkSlicerCubeAxesDisplayableManager3d);

//--------------------------------------
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCamera.h>
#include <vtkColor.h>
#include <vtkCubeAxesActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSuperquadricSource.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

#include "vtkMRMLScene.h"
//--------------------------------------

class vtkSlicerCubeAxesDisplayableManager3d::vtkInternal
{
public:
  vtkInternal(vtkSlicerCubeAxesDisplayableManager3d * external);
  ~vtkInternal();

  void ShowCubeAxesActor(bool show);

  vtkNew<vtkCubeAxesActor> cubeAxesActor;

  vtkSlicerCubeAxesDisplayableManager3d* External;

  qSlicerApplication* app;
};

// vtkInternal methods
vtkSlicerCubeAxesDisplayableManager3d::vtkInternal::vtkInternal(vtkSlicerCubeAxesDisplayableManager3d * external)
{
//  this->cubeAxesActor = vtkNew<vtkCubeAxesActor>();
  this->External = external;
  this->app = qSlicerApplication::application();
}

vtkSlicerCubeAxesDisplayableManager3d::vtkInternal::~vtkInternal(){}

void vtkSlicerCubeAxesDisplayableManager3d::vtkInternal::ShowCubeAxesActor(bool show)
{
  cubeAxesActor->SetVisibility(show);
}

vtkSlicerCubeAxesDisplayableManager3d::vtkSlicerCubeAxesDisplayableManager3d()
{
  this->Internal = new vtkInternal(this);
  this->Internal->cubeAxesActor->SetUse2DMode(1);
//  this->Internal->cubeAxesActor->SetVisibility(false);



//  // Define colors for this example
//  vtkNew<vtkNamedColors> colors;

//  vtkColor3d backgroundColor = colors->GetColor3d("DarkSlateGray");
//  vtkColor3d actorColor = colors->GetColor3d("Tomato");
//  vtkColor3d axis1Color = colors->GetColor3d("Salmon");
//  vtkColor3d axis2Color = colors->GetColor3d("PaleGreen");
//  vtkColor3d axis3Color = colors->GetColor3d("LightSkyBlue");

//  // Create a superquadric
//  vtkNew<vtkSuperquadricSource> superquadricSource;
//  superquadricSource->SetPhiRoundness(3.1);
//  superquadricSource->SetThetaRoundness(1.0);
//  superquadricSource->Update(); // needed to GetBounds later

//  vtkNew<vtkPolyDataMapper> mapper;
//  mapper->SetInputConnection(superquadricSource->GetOutputPort());

//  vtkNew<vtkActor> superquadricActor;
//  superquadricActor->SetMapper(mapper);
//  superquadricActor->GetProperty()->SetDiffuseColor(actorColor.GetData());
//  superquadricActor->GetProperty()->SetDiffuse(.7);
//  superquadricActor->GetProperty()->SetSpecular(.7);
//  superquadricActor->GetProperty()->SetSpecularPower(50.0);

//  vtkNew<vtkCubeAxesActor> cubeAxesActor;
//  cubeAxesActor->SetUseTextActor3D(1);
//  cubeAxesActor->SetBounds(superquadricSource->GetOutput()->GetBounds());
////  cubeAxesActor->SetCamera(this->GetRenderer()->GetActiveCamera());
//  cubeAxesActor->GetTitleTextProperty(0)->SetColor(axis1Color.GetData());
//  cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);
//  cubeAxesActor->GetLabelTextProperty(0)->SetColor(axis1Color.GetData());

//  cubeAxesActor->GetTitleTextProperty(1)->SetColor(axis2Color.GetData());
//  cubeAxesActor->GetLabelTextProperty(1)->SetColor(axis2Color.GetData());

//  cubeAxesActor->GetTitleTextProperty(2)->SetColor(axis3Color.GetData());
//  cubeAxesActor->GetLabelTextProperty(2)->SetColor(axis3Color.GetData());

//  cubeAxesActor->DrawXGridlinesOn();
//  cubeAxesActor->DrawYGridlinesOn();
//  cubeAxesActor->DrawZGridlinesOn();
//#if VTK_MAJOR_VERSION == 6
//  cubeAxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
//#endif
//#if VTK_MAJOR_VERSION > 6
//  cubeAxesActor->SetGridLineLocation(cubeAxesActor->VTK_GRID_LINES_FURTHEST);
//#endif

//  cubeAxesActor->XAxisMinorTickVisibilityOff();
//  cubeAxesActor->YAxisMinorTickVisibilityOff();
//  cubeAxesActor->ZAxisMinorTickVisibilityOff();

//  cubeAxesActor->SetFlyModeToStaticEdges();
//  //  this->GetRenderer()->AddActor(cubeAxesActor);
//  //  this->GetRenderer()->AddActor(superquadricActor);

//  auto scene = this->GetMRMLScene();

//  auto r = this->GetRenderer();

//  int a = 0;
}

vtkSlicerCubeAxesDisplayableManager3d::~vtkSlicerCubeAxesDisplayableManager3d()
{
  delete this->Internal;
}

void vtkSlicerCubeAxesDisplayableManager3d::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkSlicerCubeAxesDisplayableManager3d::Create()
{
  this->Superclass::Create();
}

void vtkSlicerCubeAxesDisplayableManager3d::
OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller)
{
}

void vtkSlicerCubeAxesDisplayableManager3d::showAxes(bool val) {
  this->Internal->cubeAxesActor->SetUseTextActor3D(1);
  this->Internal->cubeAxesActor->SetUse2DMode(1);
//  cubeAxesActor->SetBounds(sphereSource->GetOutput()->GetBounds());
//  this->Internal->cubeAxesActor->SetBounds(-50, 50, -50, 50, -50, 50);
  this->Internal->cubeAxesActor->SetCamera(this->GetRenderer()->GetActiveCamera());
  this->Internal->cubeAxesActor->GetTitleTextProperty(0)->SetFontSize(48);

  this->Internal->cubeAxesActor->DrawXGridlinesOn();
  this->Internal->cubeAxesActor->DrawYGridlinesOn();
  this->Internal->cubeAxesActor->DrawZGridlinesOn();
#if VTK_MAJOR_VERSION == 6
  this->Internal->cubeAxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
#endif
#if VTK_MAJOR_VERSION > 6
  this->Internal->cubeAxesActor->SetGridLineLocation(
        this->Internal->cubeAxesActor->VTK_GRID_LINES_FURTHEST);
#endif

  this->Internal->cubeAxesActor->XAxisMinorTickVisibilityOff();
  this->Internal->cubeAxesActor->YAxisMinorTickVisibilityOff();
  this->Internal->cubeAxesActor->ZAxisMinorTickVisibilityOff();


//  this->Internal->cubeAxesActor->SetBounds(-100, 100, -100, 100, -100, 100);

//  auto volumeNode = vtkMRMLVolumeNode::SafeDownCast(GetSelectionNode());
//  auto volumeNode = vtkMRMLVolumeNode::SafeDownCast(GetInteractionNode());
//  vtkMRMLNode* node = GetMRMLDisplayableNode();
//  vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast(node);

  std::string vSelName, vInteractName, vDispName;
  auto vSel = GetSelectionNode();
  auto vSelVolumeID = vSel->GetActiveVolumeID();
  auto vInteract = GetInteractionNode();
  auto vDisp = GetMRMLDisplayableNode();

  if (vSel)
    vSelName = vSel->GetName();
  if (vInteract)
    vInteractName = vInteract->GetName();
  if (vDisp)
    vDispName = vDisp->GetName();

  auto pickedNode = GetPickedNodeID();

  vtkMRMLVolumeRenderingDisplayableManager* volumeManager =
      dynamic_cast<vtkMRMLVolumeRenderingDisplayableManager*>(
        this->Internal->app->layoutManager()->threeDWidget(0)->threeDView()->
        displayableManagerByClassName("vtkMRMLVolumeRenderingDisplayableManager"));
  if (!volumeManager)
    return;

  std::string vSelName1, vInteractName1, vDispName1;
  auto vSel1 = volumeManager->GetSelectionNode();
  auto vSelVolumeID1 = vSel1->GetActiveVolumeID();
  auto vInteract1 = volumeManager->GetInteractionNode();

  if (vSel1)
    vSelName1 = vSel1->GetName();
  if (vInteract1)
    vInteractName1 = vInteract1->GetName();

  auto pickedNode1 = volumeManager->GetPickedNodeID();

  vtkMRMLVolumeNode* volumeNode = dynamic_cast<vtkMRMLVolumeNode*>(vSel);
  if (!volumeNode)
    return;

  double bounds[6];
  volumeNode->GetBounds(bounds);
  this->Internal->cubeAxesActor->SetBounds(bounds);
//  b->GetBounds()

  this->Internal->cubeAxesActor->SetVisibility(val);
  this->GetRenderer()->AddActor(this->Internal->cubeAxesActor);

}
