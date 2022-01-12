// SceneFilter Logic includes
#include "vtkSlicerSceneFilterLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLDisplayableNode.h>
#include <vtkMRMLDisplayNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// h5geo includes
#define H5GEO_USE_GDAL
#include <h5geo/h5core.h>

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

  void Execute(vtkObject *caller, unsigned long event, void*) override{
    vtkMRMLDisplayableNode* dispNode =
        vtkMRMLDisplayableNode::SafeDownCast(caller);

    if (!dispNode)
      return;

    if (event == vtkMRMLDisplayableNode::DisplayModifiedEvent){
      Logic->filter(dispNode);
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

  this->ObserveExistingNodes();
}

vtkSlicerSceneFilterLogic::vtkInternal::~vtkInternal(){}

void vtkSlicerSceneFilterLogic::vtkInternal::ObserveExistingNodes()
{
  if (!this->External->GetMRMLScene()){
    vtkErrorWithObjectMacro(
          this->External,
          "vtkSlicerSceneFilterLogic::vtkInternal::ObserveExistingNodes: " <<
          "unable to get the scene");
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
      return;

    dispNode->AddObserver(
          vtkMRMLDisplayableNode::DisplayModifiedEvent,
          this->Observer);
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

  filter();
}

//----------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::setDomainFilter(const std::string& domain)
{
  h5geo::sr::setDomain(domain);

  h5geo::Domain domainEnum = h5geo::sr::getDomainEnum();
  InvokeEvent(
        vtkSlicerSceneFilterLogic::DomainChangedEvent,
        static_cast<void*>(&domainEnum));
}

//----------------------------------------------------------------------------
std::string vtkSlicerSceneFilterLogic::getDomainFilter()
{
  return h5geo::sr::getDomain();
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::filter()
{
  // important or app may fail to load
  if (!GetMRMLScene()){
    vtkErrorMacro("vtkSlicerSceneFilterLogic::filter: " <<
                  "unable to get the scene");
    return;
  }

  vtkCollection* nodes = GetMRMLScene()->GetNodes();
  vtkObject* object = nullptr;
  vtkCollectionSimpleIterator it;
  for (nodes->InitTraversal(it); (object = nodes->GetNextItemAsObject(it));)
  {
    vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(object);
    if (!dispNode)
      continue;

    filter(dispNode);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerSceneFilterLogic::filter(vtkMRMLDisplayableNode* node)
{
  if (!node || !node->GetDisplayVisibility())
    return;

  std::string attrVal;
  if (!this->Internal->attributeName.empty() &&
      node->GetAttribute(this->Internal->attributeName.c_str()))
    attrVal = node->GetAttribute(this->Internal->attributeName.c_str());

  std::string nodeDomain;
  if (node->GetAttribute("Domain"))
    nodeDomain = node->GetAttribute("Domain");

  std::string domain = getDomainFilter();
  if ((attrVal.empty() ||
       this->Internal->attributeValue.empty() ||
       attrVal == this->Internal->attributeValue) &&
      (nodeDomain.empty() ||
       domain == "ANY" ||
       domain.empty() ||
       nodeDomain == domain))
    return;

  node->SetDisplayVisibility(false);
  node->InvokeEvent(
        vtkMRMLDisplayableNode::DisplayModifiedEvent,
        node);
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