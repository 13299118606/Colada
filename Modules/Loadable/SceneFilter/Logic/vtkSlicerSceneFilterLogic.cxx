// SceneFilter Logic includes
#include "vtkSlicerSceneFilterLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLVolumeNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// h5geo includes
#include <h5geo/h5util.h>

// STD includes
#include <cassert>

//---------------------------------------------------------------------------
class vtkSlicerSceneFilterLogicObserver
    : public vtkCommand
{
public:
  static vtkSlicerSceneFilterLogicObserver *New(){
    return new vtkSlicerSceneFilterLogicObserver;
  }

  vtkSlicerSceneFilterLogicObserver(){
    this->Logic = nullptr;
  }

  void Execute(vtkObject *caller, unsigned long event, void* val) override{
    // very necessary to avoid segfault when closing the app
    if (!Logic)
      return;

    vtkMRMLDisplayableNode* dispNode =
        vtkMRMLDisplayableNode::SafeDownCast(caller);

    if (!dispNode)
      return;

    if (event == vtkMRMLDisplayableNode::DisplayModifiedEvent){
      Logic->filter(dispNode, true);
    }
  }

  vtkWeakPointer<vtkSlicerSceneFilterLogic> Logic;
};

//---------------------------------------------------------------------------
class vtkSlicerSceneFilterLogic::vtkInternal
{
public:
  vtkInternal(vtkSlicerSceneFilterLogic * external);
  ~vtkInternal();

  void ObserveExistingNodes();
  void RemoveObserverFromNodes();

  vtkSlicerSceneFilterLogic* External;
  vtkSmartPointer<vtkSlicerSceneFilterLogicObserver> Observer;
  std::string attributeName, attributeValue;
};

// vtkInternal methods
vtkSlicerSceneFilterLogic::
vtkInternal::
vtkInternal(vtkSlicerSceneFilterLogic * external)
{
  this->External = external;
  this->Observer = vtkSmartPointer<vtkSlicerSceneFilterLogicObserver>::New();
  this->Observer->Logic = this->External;
}

vtkSlicerSceneFilterLogic::vtkInternal::~vtkInternal()
{
  // necessary to prevent memory leaks
  this->RemoveObserverFromNodes();
}

void vtkSlicerSceneFilterLogic::vtkInternal::ObserveExistingNodes()
{
  if (!this->External->GetMRMLScene()){
    vtkErrorWithObjectMacro(
          this->External,
          "vtkSlicerSceneFilterLogic::vtkInternal::ObserveExistingNodes: " <<
          "Unable to get the Scene");
    return;
  }

  vtkCollection* nodes = this->External->GetMRMLScene()->GetNodes();
  vtkObject* object = nullptr;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it); (object = nodes->GetNextItemAsObject(it));)
  {
    vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(object);
    if (!dispNode)
      continue;

    if (dispNode->HasObserver(
          vtkMRMLDisplayableNode::DisplayModifiedEvent,
          this->Observer))
      continue;

    dispNode->AddObserver(
          vtkMRMLDisplayableNode::DisplayModifiedEvent,
          this->Observer);
  }
}

void vtkSlicerSceneFilterLogic::vtkInternal::RemoveObserverFromNodes()
{
  if (!this->External->GetMRMLScene()){
    vtkErrorWithObjectMacro(
          this->External,
          "vtkSlicerSceneFilterLogic::vtkInternal::RemoveObserverFromNodes: " <<
          "Unable to get the Scene");
    return;
  }

  vtkCollection* nodes = this->External->GetMRMLScene()->GetNodes();
  vtkObject* object = nullptr;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it); (object = nodes->GetNextItemAsObject(it));)
  {
    vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(object);
    if (!dispNode)
      continue;

    if (!dispNode->HasObserver(
          vtkMRMLDisplayableNode::DisplayModifiedEvent,
          this->Observer))
      continue;

    dispNode->RemoveObserver(this->Observer);
  }
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerSceneFilterLogic);

//----------------------------------------------------------------------------
vtkSlicerSceneFilterLogic::vtkSlicerSceneFilterLogic()
{
  this->Internal = new vtkInternal(this);
}

//----------------------------------------------------------------------------
vtkSlicerSceneFilterLogic::~vtkSlicerSceneFilterLogic()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::setAttributeFilter(
    const std::string& nodeAttr,
    const std::string& attrValue)
{
  this->Internal->attributeName = nodeAttr;
  this->Internal->attributeValue = attrValue;

  if (nodeAttr.empty() ||
      attrValue.empty())
    return;

  filter(false);
}

//----------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::setDomainFilter(const std::string& domain)
{
  h5geo::sr::setDomain(domain);

  if (!GetMRMLScene())
    return;

  h5geo::Domain domainEnum = h5geo::sr::getDomainEnum();
  // invoke event first so that nodes could apply transformations
  // and change the domain
  InvokeEvent(
        vtkSlicerSceneFilterLogic::DomainChangedEvent,
        static_cast<void*>(&domainEnum));

  // now filter (show/hide) nodes with inapropriate domain
  filter(false);
}

//----------------------------------------------------------------------------
std::string vtkSlicerSceneFilterLogic::getDomainFilter()
{
  return h5geo::sr::getDomain();
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::filter(bool hideOnly)
{
  // important or app may fail to load
  if (!GetMRMLScene()){
    vtkErrorMacro("vtkSlicerSceneFilterLogic::filter: " <<
                  "Unable to get the Scene");
    return;
  }

  vtkCollection* nodes = GetMRMLScene()->GetNodes();
  vtkObject* object = nullptr;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it); (object = nodes->GetNextItemAsObject(it));)
  {
    vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(object);
    // some nodes are not shown and they are used by Slicer
    // even vtkMRMLModelNode can be such node ('Red Volume Slice' for example)
    if (!dispNode || dispNode->GetHideFromEditors())
      continue;

    filter(dispNode, hideOnly);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::filter(
    vtkMRMLDisplayableNode* node,
    bool hideOnly)
{
  if (!node)
    return;

  std::string attrVal;
  if (!this->Internal->attributeName.empty() &&
      node->GetAttribute(this->Internal->attributeName.c_str()))
    attrVal = node->GetAttribute(this->Internal->attributeName.c_str());

  std::string nodeDomain;
  if (node->GetAttribute("Domain"))
    nodeDomain = node->GetAttribute("Domain");

  std::string domain = getDomainFilter();
  if ((this->Internal->attributeValue.empty() ||
       this->Internal->attributeValue == attrVal) &&
      (domain == "ANY" ||
       nodeDomain == domain)){
    if (!node->GetDisplayVisibility() &&
        !hideOnly){
      node->SetDisplayVisibility(true);
      node->InvokeEvent(
            vtkMRMLDisplayableNode::DisplayModifiedEvent,
            node);
    }
  } else {
    if (node->GetDisplayVisibility()){
      node->SetDisplayVisibility(false);
      node->InvokeEvent(
            vtkMRMLDisplayableNode::DisplayModifiedEvent,
            node);
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  // as this function is invoked when the scene is already added
  // we can use it to observe existing nodes
  this->Internal->ObserveExistingNodes();
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  vtkMRMLDisplayableNode* dispNode =
      vtkMRMLDisplayableNode::SafeDownCast(node);

  if (!dispNode)
    return;

  if (dispNode->HasObserver(
        vtkMRMLDisplayableNode::DisplayModifiedEvent,
        this->Internal->Observer))
    return;

  dispNode->AddObserver(
        vtkMRMLDisplayableNode::DisplayModifiedEvent,
        this->Internal->Observer);
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
