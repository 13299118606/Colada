// MRMLDisplayableManager includes
#include "vtkMRMLAxesDisplayableManager3D.h"

// MRML includes
#include <vtkMRMLLogic.h>
#include <vtkMRMLDisplayableNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkObjectFactory.h>
#include <vtkCubeAxesActor.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>

// STD includes
#include <sstream>

// SlicerQt includes
#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>

// vtkSlicer includes
#include <vtkSlicerApplicationLogic.h>

vtkStandardNewMacro(vtkMRMLAxesDisplayableManager3D);

//--------------------------------------

class vtkMRMLAxesDisplayableManager3D::vtkInternal
    : public vtkCommand
{
public:
  vtkInternal(vtkMRMLAxesDisplayableManager3D * external);
  virtual ~vtkInternal();

  void SetupAxes();
  bool SetAndObserveMRMLNodeBounds(vtkMRMLDisplayableNode* node);
  void Execute(vtkObject *caller, unsigned long event, void*) override;

  vtkNew<vtkCubeAxesActor> AxesActor;
  vtkMRMLAxesDisplayableManager3D* External;
  qSlicerApplication* app;
  vtkMRMLDisplayableNode* ObservedNode = nullptr;
};

// vtkInternal methods
vtkMRMLAxesDisplayableManager3D::
vtkInternal::
vtkInternal(vtkMRMLAxesDisplayableManager3D * external)
{
  this->External = external;
  this->app = qSlicerApplication::application();
}

vtkMRMLAxesDisplayableManager3D::vtkInternal::~vtkInternal(){}

void vtkMRMLAxesDisplayableManager3D::vtkInternal::SetupAxes()
{
  this->AxesActor->SetUse2DMode(1);
  this->AxesActor->SetUseTextActor3D(0);
  this->AxesActor->SetCamera(this->External->GetRenderer()->GetActiveCamera());

  this->AxesActor->DrawXGridlinesOn();
  this->AxesActor->DrawYGridlinesOn();
  this->AxesActor->DrawZGridlinesOn();
#if VTK_MAJOR_VERSION == 6
  this->AxesActor->SetGridLineLocation(VTK_GRID_LINES_FURTHEST);
#endif
#if VTK_MAJOR_VERSION > 6
  this->AxesActor->SetGridLineLocation(
        this->AxesActor->VTK_GRID_LINES_FURTHEST);
#endif

  this->AxesActor->XAxisMinorTickVisibilityOff();
  this->AxesActor->YAxisMinorTickVisibilityOff();
  this->AxesActor->ZAxisMinorTickVisibilityOff();
  this->AxesActor->SetVisibility(false); // don't need to show it on its own
  this->External->GetRenderer()->AddActor(this->AxesActor);
}

bool vtkMRMLAxesDisplayableManager3D::vtkInternal::SetAndObserveMRMLNodeBounds(
    vtkMRMLDisplayableNode* node)
{
  if (!node)
    return false;

  double bounds[6];
  node->GetRASBounds(bounds);
  AxesActor->SetBounds(bounds);

  if (this->ObservedNode != nullptr)
    this->ObservedNode->RemoveObserver(this);

  node->AddObserver(
        vtkMRMLTransformableNode::TransformModifiedEvent,
        this);
  node->AddObserver(
        vtkCommand::DeleteEvent,
        this);

  this->ObservedNode = node;

  return true;
}

void vtkMRMLAxesDisplayableManager3D::vtkInternal::Execute(
    vtkObject *caller, unsigned long event, void*)
{
  vtkMRMLDisplayableNode* dispNode =
      vtkMRMLDisplayableNode::SafeDownCast(caller);

  if (!dispNode)
    return;

  if (event == vtkMRMLTransformableNode::TransformModifiedEvent){
    double bounds[6];
    dispNode->GetRASBounds(bounds);
    this->AxesActor->SetBounds(bounds);
  } else if (event == vtkCommand::DeleteEvent){
    this->ObservedNode = nullptr;
  }
}

vtkMRMLAxesDisplayableManager3D::vtkMRMLAxesDisplayableManager3D()
{
  this->Internal = new vtkInternal(this);
}

vtkMRMLAxesDisplayableManager3D::~vtkMRMLAxesDisplayableManager3D()
{
  delete this->Internal;
}

void vtkMRMLAxesDisplayableManager3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vtkMRMLAxesDisplayableManager3D::Create()
{
  this->Superclass::Create();
  // set default settings and add axes to the renderer
  this->Internal->SetupAxes();
}

bool vtkMRMLAxesDisplayableManager3D::SetAndObserveMRMLNodeBounds(vtkMRMLDisplayableNode* node)
{
  return this->Internal->SetAndObserveMRMLNodeBounds(node);
}

void vtkMRMLAxesDisplayableManager3D::UpdateFromViewNode()
{
  // View node is changed, which may mean that either the marker type (visibility), size, or orientation is changed
//  this->Internal->UpdateRulers();
  int a = 0;
}

void vtkMRMLAxesDisplayableManager3D::
OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller)
{
  // view node is changed
  this->UpdateFromViewNode();
}

void vtkMRMLAxesDisplayableManager3D::UpdateFromRenderer()
{
  // Rendering is performed, so let's re-render the marker with up-to-date orientation
//  this->Internal->UpdateRulers();
  int a = 0;
}

vtkCubeAxesActor* vtkMRMLAxesDisplayableManager3D::GetCubeAxesActor()
{
  return this->Internal->AxesActor;
}
