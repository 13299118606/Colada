class vtkMRMLSliceNode;

class qSlicerViewControllersModule{
  void readDefaultSliceViewSettings(vtkMRMLSliceNode* defaultViewNode);
}

void qSlicerViewControllersModule::readDefaultSliceViewSettings(vtkMRMLSliceNode* defaultViewNode)
{
  if (!defaultViewNode)
    {
    qCritical() << Q_FUNC_INFO << " failed: defaultViewNode is invalid";
    return;
    }
  QSettings settings;
  settings.beginGroup("DefaultSliceView");
  readCommonViewSettings(defaultViewNode, settings);
}