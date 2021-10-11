class vtkMRMLNode;

class vtkMRMLAbstractViewNode{
  virtual void Reset(vtkMRMLNode* defaultNode) override;
}

void vtkMRMLAbstractViewNode::Reset(vtkMRMLNode* defaultNode)
{
  // The LayoutName is preserved by vtkMRMLNode::Reset, however the layout
  // label (typically associated with the layoutName) is not preserved
  // automatically.
  // This require a custom behavior implemented here.
  std::string layoutLabel = this->GetLayoutLabel() ? this->GetLayoutLabel() : "";
  int viewGroup = this->GetViewGroup();
  this->Superclass::Reset(defaultNode);
  this->DisableModifiedEventOn();
  this->SetLayoutLabel(layoutLabel.c_str());
  this->SetViewGroup(viewGroup);
  // this->AxisLabels->Reset();
  // vtkMRMLAbstractViewNode* defaultViewNode = vtkMRMLAbstractViewNode::SafeDownCast(defaultNode);
  // if (defaultViewNode){
  //   for (int i=0; i<vtkMRMLAbstractViewNode::AxisLabelsCount; i++)
  //   {
  //     this->AxisLabels->InsertNextValue(defaultViewNode->GetAxisLabel(i));
  //   }
  // } else {
  // for (int i=0; i<vtkMRMLAbstractViewNode::AxisLabelsCount; i++)
  //   {
  //   this->AxisLabels->InsertNextValue(DEFAULT_AXIS_LABELS[i]);
  //   }
  // }
  this->DisableModifiedEventOff();
}