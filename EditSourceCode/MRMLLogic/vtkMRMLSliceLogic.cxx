class vtkMRMLSliceLogic {
  void ResizeSliceNode(double newWidth, double newHeight);
}

void vtkMRMLSliceLogic::ResizeSliceNode(double newWidth, double newHeight)
{
  if (!this->SliceNode)
    {
    return;
    }

  // New size must be the active slice vtkRenderer size. It's the same than the window
  // if the layout is 1x1.
  newWidth /= this->SliceNode->GetLayoutGridColumns();
  newHeight /= this->SliceNode->GetLayoutGridRows();

  // The following was previously in SliceSWidget.tcl
  double sliceStep = this->SliceSpacing[2];
  int oldDimensions[3];
  this->SliceNode->GetDimensions(oldDimensions);
  double oldFOV[3];
  this->SliceNode->GetFieldOfView(oldFOV);

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
  newFOV[2] = sliceStep * oldDimensions[2];
  // double windowAspect = (newWidth != 0. ? newHeight / newWidth : 1.);
  // double planeAspect = (newFOV[0] != 0. ? newFOV[1] / newFOV[0] : 1.);
  // if (windowAspect != planeAspect)
  //   {
  //   newFOV[0] = (windowAspect != 0. ? newFOV[1] / windowAspect : newFOV[0]);
  //   }
  int disabled = this->SliceNode->StartModify();
  this->SliceNode->SetDimensions(newWidth, newHeight, oldDimensions[2]);
  this->SliceNode->SetFieldOfView(newFOV[0], newFOV[1], newFOV[2]);
  this->SliceNode->EndModify(disabled);
}