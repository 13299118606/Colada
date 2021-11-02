class vtkMRMLCameraWidget {
protected:
  bool ProcessTranslate(vtkMRMLInteractionEventData* eventData);
}

bool vtkMRMLCameraWidget::ProcessTranslate(vtkMRMLInteractionEventData* eventData)
{
  if (!this->Renderer || !eventData)
    {
    return false;
    }

  const int* eventPosition = eventData->GetDisplayPosition();
  int dx = eventPosition[0] - this->PreviousEventPosition[0];
  int dy = eventPosition[1] - this->PreviousEventPosition[1];
  if (dx == 0 && dy == 0)
    {
    return true;
    }

  // Calculate the focal depth since we'll be using it a lot
  vtkCamera* camera = this->GetCamera();
  if (!camera)
    {
    return false;
    }

  double viewFocus[4] = { 0.0, 0.0, 0.0, 1.0 };
  camera->GetFocalPoint(viewFocus);
  vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,
    viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);

  double focalDepth = viewFocus[2];

  double newPickPoint[4] = { 0.0, 0.0, 0.0, 1.0 };
  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer,
    (double)eventPosition[0], (double)eventPosition[1], focalDepth, newPickPoint);

  // Has to recalc old mouse point since the viewport has moved,
  // so can't move it outside the loop

  double oldPickPoint[4] = { 0.0, 0.0, 0.0, 1.0 };
  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer,
    (double)this->PreviousEventPosition[0], (double)this->PreviousEventPosition[1],
    focalDepth, oldPickPoint);

  // Camera motion is reversed

  vtkMatrix4x4* M = camera->GetModelTransformMatrix();
  double motionVector[3] =
    {
    (oldPickPoint[0] - newPickPoint[0]) * M->GetElement(0,0),
    (oldPickPoint[1] - newPickPoint[1]) * M->GetElement(1,1),
    (oldPickPoint[2] - newPickPoint[2]) * M->GetElement(2,2)
    };
  camera->GetFocalPoint(viewFocus);

  double viewPoint[3] = { 0.0 };
  camera->GetPosition(viewPoint);

  bool wasCameraNodeModified = this->CameraModifyStart();

  camera->SetFocalPoint(motionVector[0] + viewFocus[0],
    motionVector[1] + viewFocus[1],
    motionVector[2] + viewFocus[2]);

  camera->SetPosition(motionVector[0] + viewPoint[0],
    motionVector[1] + viewPoint[1],
    motionVector[2] + viewPoint[2]);

  this->CameraModifyEnd(wasCameraNodeModified, true, true);

  this->PreviousEventPosition[0] = eventPosition[0];
  this->PreviousEventPosition[1] = eventPosition[1];

  return true;
}