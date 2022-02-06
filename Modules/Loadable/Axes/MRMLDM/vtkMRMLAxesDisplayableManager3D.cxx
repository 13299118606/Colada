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
#include <vtkProperty.h>
#include <vtkTextProperty.h>

// STD includes
#include <sstream>

vtkStandardNewMacro(vtkMRMLAxesDisplayableManager3D);

//---------------------------------------------------------------------------
class vtkMRMLAxesDisplayableManager3DObserver
    : public vtkCommand
{
public:
  static vtkMRMLAxesDisplayableManager3DObserver *New(){
    return new vtkMRMLAxesDisplayableManager3DObserver;
  }

  vtkMRMLAxesDisplayableManager3DObserver(){
    this->DisplayableManager = nullptr;
  }

  void Execute(vtkObject *caller, unsigned long event, void*) override{
    // very necessary to avoid segfault when closing the app
    if (!DisplayableManager)
      return;

    vtkMRMLDisplayableNode* dispNode =
        vtkMRMLDisplayableNode::SafeDownCast(caller);

    if (!dispNode)
      return;

    if (event == vtkMRMLTransformableNode::TransformModifiedEvent){
      double bounds[6];
      dispNode->GetRASBounds(bounds);
      this->DisplayableManager->GetCubeAxesActor()->SetBounds(bounds);
    } else if (event == vtkCommand::DeleteEvent){
      this->DisplayableManager->ReleaseObservedNode();
    }
  }

  vtkWeakPointer<vtkMRMLAxesDisplayableManager3D> DisplayableManager;
};

//---------------------------------------------------------------------------
class vtkMRMLAxesDisplayableManager3D::vtkInternal
{
public:
  vtkInternal(vtkMRMLAxesDisplayableManager3D * external);
  ~vtkInternal();

  void SetupAxes();
  bool SetAndObserveMRMLNodeBounds(vtkMRMLDisplayableNode* node);

  vtkNew<vtkCubeAxesActor> AxesActor;
  vtkMRMLAxesDisplayableManager3D* External;
  vtkMRMLDisplayableNode* ObservedNode = nullptr;
  vtkSmartPointer<vtkMRMLAxesDisplayableManager3DObserver> Observer;
  bool Visibility;
};

// vtkInternal methods
vtkMRMLAxesDisplayableManager3D::
vtkInternal::
vtkInternal(vtkMRMLAxesDisplayableManager3D * external)
{
  this->External = external;
  this->Observer = vtkSmartPointer<vtkMRMLAxesDisplayableManager3DObserver>::New();
  this->Observer->DisplayableManager = this->External;
}

vtkMRMLAxesDisplayableManager3D::vtkInternal::~vtkInternal(){}

void vtkMRMLAxesDisplayableManager3D::vtkInternal::SetupAxes()
{
  this->AxesActor->SetUse2DMode(1);
  this->AxesActor->SetUseTextActor3D(0);
  this->AxesActor->SetCamera(this->External->GetRenderer()->GetActiveCamera());

  this->AxesActor->SetXAxisVisibility(true);
  this->AxesActor->SetYAxisVisibility(true);
  this->AxesActor->SetZAxisVisibility(true);

  this->AxesActor->DrawXGridlinesOff();
  this->AxesActor->DrawYGridlinesOff();
  this->AxesActor->DrawZGridlinesOff();

  this->AxesActor->DrawXInnerGridlinesOff();
  this->AxesActor->DrawYInnerGridlinesOff();
  this->AxesActor->DrawZInnerGridlinesOff();
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

  // axes
  this->AxesActor->GetXAxesLinesProperty()->SetLineWidth(5);
  this->AxesActor->GetYAxesLinesProperty()->SetLineWidth(5);
  this->AxesActor->GetZAxesLinesProperty()->SetLineWidth(5);

  this->AxesActor->GetXAxesLinesProperty()->SetColor(1,0,0);
  this->AxesActor->GetYAxesLinesProperty()->SetColor(0,1,0);
  this->AxesActor->GetZAxesLinesProperty()->SetColor(0,0,1);

  this->AxesActor->GetXAxesLinesProperty()->RenderLinesAsTubesOn();
  this->AxesActor->GetYAxesLinesProperty()->RenderLinesAsTubesOn();
  this->AxesActor->GetZAxesLinesProperty()->RenderLinesAsTubesOn();

  this->AxesActor->SetXTitle("X");
  this->AxesActor->SetYTitle("Y");
  this->AxesActor->SetZTitle("Z");

  // label (numbers)
  this->AxesActor->GetLabelTextProperty(0)->SetColor(1,0,0);
  this->AxesActor->GetLabelTextProperty(1)->SetColor(0,1,0);
  this->AxesActor->GetLabelTextProperty(2)->SetColor(0,0,1);

  this->AxesActor->GetLabelTextProperty(0)->SetFontSize(12);
  this->AxesActor->GetLabelTextProperty(1)->SetFontSize(12);
  this->AxesActor->GetLabelTextProperty(2)->SetFontSize(12);

  // axis text
  this->AxesActor->GetTitleTextProperty(0)->SetColor(1,0,0);
  this->AxesActor->GetTitleTextProperty(1)->SetColor(0,1,0);
  this->AxesActor->GetTitleTextProperty(2)->SetColor(0,0,1);

  this->AxesActor->GetTitleTextProperty(0)->SetFontSize(12);
  this->AxesActor->GetTitleTextProperty(1)->SetFontSize(12);
  this->AxesActor->GetTitleTextProperty(2)->SetFontSize(12);

  // grid
  this->AxesActor->GetXAxesGridlinesProperty()->SetLineWidth(1);
  this->AxesActor->GetYAxesGridlinesProperty()->SetLineWidth(1);
  this->AxesActor->GetZAxesGridlinesProperty()->SetLineWidth(1);

  this->AxesActor->GetXAxesGridlinesProperty()->SetColor(1,0,0);
  this->AxesActor->GetYAxesGridlinesProperty()->SetColor(0,1,0);
  this->AxesActor->GetZAxesGridlinesProperty()->SetColor(0,0,1);

  // inner grid
  this->AxesActor->GetXAxesInnerGridlinesProperty()->SetLineWidth(1);
  this->AxesActor->GetYAxesInnerGridlinesProperty()->SetLineWidth(1);
  this->AxesActor->GetZAxesInnerGridlinesProperty()->SetLineWidth(1);

  this->AxesActor->GetXAxesInnerGridlinesProperty()->SetColor(1,0,0);
  this->AxesActor->GetYAxesInnerGridlinesProperty()->SetColor(0,1,0);
  this->AxesActor->GetZAxesInnerGridlinesProperty()->SetColor(0,0,1);

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
    this->ObservedNode->RemoveObserver(this->Observer);

  node->AddObserver(
        vtkMRMLTransformableNode::TransformModifiedEvent,
        this->Observer);
  node->AddObserver(
        vtkCommand::DeleteEvent,
        this->Observer);

  this->ObservedNode = node;

  return true;
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

void vtkMRMLAxesDisplayableManager3D::SetAndObserveMRMLNodeBounds(vtkMRMLDisplayableNode* node)
{
  this->Internal->SetAndObserveMRMLNodeBounds(node);
}

void vtkMRMLAxesDisplayableManager3D::UpdateFromViewNode()
{
  // View node is changed, which may mean that either the marker type (visibility), size, or orientation is changed
//  this->Internal->UpdateRulers();
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
}

vtkCubeAxesActor* vtkMRMLAxesDisplayableManager3D::GetCubeAxesActor()
{
  return this->Internal->AxesActor;
}

vtkMRMLDisplayableNode* vtkMRMLAxesDisplayableManager3D::GetObservedNode()
{
  return this->Internal->ObservedNode;
}

void vtkMRMLAxesDisplayableManager3D::ReleaseObservedNode()
{
  this->Internal->ObservedNode = nullptr;
  this->Internal->AxesActor->SetVisibility(false);
}
