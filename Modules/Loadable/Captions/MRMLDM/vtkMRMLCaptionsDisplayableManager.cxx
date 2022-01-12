// MRMLDisplayableManager includes
#include "vtkMRMLCaptionsDisplayableManager.h"

// MRML includes
#include <vtkMRMLAbstractViewNode.h>
#include <vtkMRMLLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLViewNode.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLMarkupsNode.h>

// VTK includes
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLCaptionsDisplayableManager);

//---------------------------------------------------------------------------
class vtkMRMLCaptionsDisplayableManagerObserver
    : public vtkCommand
{
public:
  static vtkMRMLCaptionsDisplayableManagerObserver *New(){
    return new vtkMRMLCaptionsDisplayableManagerObserver;
  }

  vtkMRMLCaptionsDisplayableManagerObserver(){
    this->DisplayableManager = nullptr;
  }

  void Execute(vtkObject *caller, unsigned long event, void*) override{
    vtkMRMLDisplayableNode* dispNode =
        vtkMRMLDisplayableNode::SafeDownCast(caller);

    if (!dispNode)
      return;

    if (event == vtkMRMLTransformableNode::TransformModifiedEvent){
      this->DisplayableManager->SetRelativePosition(dispNode);
    } else if (vtkMRMLDisplayableNode::DisplayModifiedEvent){
      this->DisplayableManager->SetCaptionVisibility(dispNode);
      this->DisplayableManager->SetCaption(dispNode);
    } else if (vtkMRMLDisplayableNode::IDChangedEvent){
      this->DisplayableManager->SetCaption(dispNode);
    }
  }

  vtkWeakPointer<vtkMRMLCaptionsDisplayableManager> DisplayableManager;
};

//---------------------------------------------------------------------------
class vtkMRMLCaptionsDisplayableManager::vtkInternal
{
public:
  vtkInternal(vtkMRMLCaptionsDisplayableManager * external);
  ~vtkInternal();

  void SetAndObserveMRMLNode(vtkMRMLDisplayableNode* node);
  void ReleaseObservedNode(vtkMRMLDisplayableNode* node);

  vtkMRMLCaptionsDisplayableManager* External;

  std::map<vtkMRMLDisplayableNode*,
  std::pair<
  vtkSmartPointer<vtkCaptionActor2D>,
  vtkSmartPointer<vtkMRMLCaptionsDisplayableManagerObserver>>>
  ObservedNodes;
  bool Visibility = true;
  bool Frame = true;
  int FrameWidth = 1;
  int FontSize = 12;
  bool Bold = true;
  bool Italic = false;
  bool Shadow = false;
  double Color[3] = {1, 1, 1};
  double FrameColor[3] = {1, 1, 1};
  double Orientation = 0;
  double Opacity = 1;
  double Position[3] = {0, 0, 0.5};
};

// vtkInternal methods
vtkMRMLCaptionsDisplayableManager::
vtkInternal::
vtkInternal(vtkMRMLCaptionsDisplayableManager * external)
{
  this->External = external;
}

vtkMRMLCaptionsDisplayableManager::vtkInternal::~vtkInternal(){}

void vtkMRMLCaptionsDisplayableManager::vtkInternal::SetAndObserveMRMLNode(
    vtkMRMLDisplayableNode* node)
{
  if (!node)
    return;

  ObservedNodes[node] =
      std::pair(
        vtkSmartPointer<vtkCaptionActor2D>::New(),
        vtkSmartPointer<vtkMRMLCaptionsDisplayableManagerObserver>::New());

  auto item = ObservedNodes[node];
  // very important to set DM to the observer
  item.second->DisplayableManager = this->External;

  item.first->SetCaption(node->GetName());
  item.first->SetVisibility(Visibility);
  item.first->GetTextActor()->SetTextScaleModeToNone(); //key: fix the font size
  item.first->GetCaptionTextProperty()->SetFontSize(FontSize);
  item.first->GetCaptionTextProperty()->SetFrame(Frame); // normal frame, can be set
  item.first->GetCaptionTextProperty()->SetShadow(Shadow);
  item.first->GetCaptionTextProperty()->SetBold(Bold);
  item.first->GetCaptionTextProperty()->SetItalic(Italic);
  item.first->GetCaptionTextProperty()->SetOrientation(Orientation);
  item.first->GetCaptionTextProperty()->SetOpacity(Opacity);
  item.first->GetCaptionTextProperty()->SetFrameWidth(FrameWidth);
  item.first->GetCaptionTextProperty()->SetColor(Color);
  item.first->GetCaptionTextProperty()->SetFrameColor(FrameColor);
  item.first->SetBorder(false); // removes outer border (too wide border)

  double bounds[6];
  node->GetRASBounds(bounds);
  item.first->SetAttachmentPoint(
        bounds[0] + (bounds[1]-bounds[0])*Position[0],
      bounds[2] + (bounds[3]-bounds[2])*Position[1],
      bounds[4] + (bounds[5]-bounds[4])*Position[2]);

  node->AddObserver(
        vtkMRMLTransformableNode::TransformModifiedEvent,
        item.second);
  node->AddObserver(
        vtkMRMLDisplayableNode::DisplayModifiedEvent,
        item.second);
  node->AddObserver(
        vtkMRMLDisplayableNode::IDChangedEvent,
        item.second);

  this->External->GetRenderer()->AddActor(item.first);
}

void vtkMRMLCaptionsDisplayableManager::vtkInternal::ReleaseObservedNode(vtkMRMLDisplayableNode* node)
{
  if (!node)
    return;

  auto item = ObservedNodes[node];

  // delete actor from renderer
  External->GetRenderer()->RemoveActor(item.first);
  ObservedNodes.erase(node);
}

//---------------------------------------------------------------------------
// vtkMRMLCaptionsDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLCaptionsDisplayableManager::vtkMRMLCaptionsDisplayableManager()
{
  this->Internal = new vtkInternal(this);
}

//---------------------------------------------------------------------------
vtkMRMLCaptionsDisplayableManager::~vtkMRMLCaptionsDisplayableManager()
{
  delete this->Internal;
}

//---------------------------------------------------------------------------
void vtkMRMLCaptionsDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLCaptionsDisplayableManager::Create()
{
  this->Superclass::Create();
}

//---------------------------------------------------------------------------
void vtkMRMLCaptionsDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* vtkNotUsed(caller))
{
}

void vtkMRMLCaptionsDisplayableManager::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  // we don't want to observe markups nodes as they have their own captions
  if (vtkMRMLMarkupsNode::SafeDownCast(node))
    return;

  vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(node);
  if (!dispNode)
    return;

  this->SetAndObserveMRMLNode(dispNode);
}

void vtkMRMLCaptionsDisplayableManager::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(node);
  if (!dispNode)
    return;

  this->ReleaseObservedNode(dispNode);
}

void vtkMRMLCaptionsDisplayableManager::SetAndObserveMRMLNode(vtkMRMLDisplayableNode* node)
{
  this->Internal->SetAndObserveMRMLNode(node);
}

void vtkMRMLCaptionsDisplayableManager::ReleaseObservedNode(vtkMRMLDisplayableNode* node)
{
  this->Internal->ReleaseObservedNode(node);
}

void vtkMRMLCaptionsDisplayableManager::SetCaption(vtkMRMLDisplayableNode* node)
{
  auto item = this->Internal->ObservedNodes[node];
  if (!item.first)
    return;

  item.first->SetCaption(node->GetName());
}

void vtkMRMLCaptionsDisplayableManager::SetCaptionVisibility(vtkMRMLDisplayableNode* node)
{
  auto item = this->Internal->ObservedNodes[node];
  if (!item.first)
    return;

  item.first->SetVisibility(
        node->GetDisplayVisibility() &&
        this->Internal->Visibility);
}

void vtkMRMLCaptionsDisplayableManager::SetCaptionsVisibility(bool val)
{
  this->Internal->Visibility = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->SetVisibility(
          it->first->GetDisplayVisibility() &&
          this->Internal->Visibility);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetFontSize(int val)
{
  this->Internal->FontSize = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetFontSize(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetBold(bool val)
{
  this->Internal->Bold = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetBold(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetItalic(bool val)
{
  this->Internal->Italic = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetItalic(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetShadow(bool val)
{
  this->Internal->Shadow = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetShadow(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetColor(const double* x)
{
  this->Internal->Color[0] = x[0];
  this->Internal->Color[1] = x[1];
  this->Internal->Color[2] = x[2];
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetColor(x);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetColor(double r, double g, double b)
{
  this->Internal->Color[0] = r;
  this->Internal->Color[1] = g;
  this->Internal->Color[2] = b;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetColor(r,g,b);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetOrientation(double val)
{
  this->Internal->Orientation = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetOrientation(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetOpacity(double val)
{
  this->Internal->Opacity = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetOpacity(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetFrame(bool val)
{
  this->Internal->Frame = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetFrame(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetFrameWidth(int val)
{
  this->Internal->FrameWidth = val;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetFrameWidth(val);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetFrameColor(const double* x)
{
  this->Internal->FrameColor[0] = x[0];
  this->Internal->FrameColor[1] = x[1];
  this->Internal->FrameColor[2] = x[2];
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetFrameColor(x);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetFrameColor(double r, double g, double b)
{
  this->Internal->FrameColor[0] = r;
  this->Internal->FrameColor[1] = g;
  this->Internal->FrameColor[2] = b;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    it->second.first->GetCaptionTextProperty()->SetFrameColor(r,g,b);
  }
}

void vtkMRMLCaptionsDisplayableManager::SetRelativePosition(vtkMRMLDisplayableNode* node)
{
  if (!node)
    return;

  double bounds[6];
  node->GetRASBounds(bounds);

  auto item = this->Internal->ObservedNodes[node];
  item.first->SetAttachmentPoint(
        bounds[0] + (bounds[1]-bounds[0])*this->Internal->Position[0],
      bounds[2] + (bounds[3]-bounds[2])*this->Internal->Position[1],
      bounds[4] + (bounds[5]-bounds[4])*this->Internal->Position[2]);
}

void vtkMRMLCaptionsDisplayableManager::SetRelativePosition(double x, double y, double z)
{
  this->Internal->Position[0] = x;
  this->Internal->Position[1] = y;
  this->Internal->Position[2] = z;
  for (auto it = this->Internal->ObservedNodes.begin();
       it != this->Internal->ObservedNodes.end(); it++)
  {
    if (!it->second.first)
      continue;

    double bounds[6];
    it->first->GetRASBounds(bounds);
    it->second.first->SetAttachmentPoint(
          bounds[0] + (bounds[1]-bounds[0])*x,
        bounds[2] + (bounds[3]-bounds[2])*y,
        bounds[4] + (bounds[5]-bounds[4])*z);
  }
}
