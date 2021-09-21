#ifndef __vtkStackLoadDisplayableManager_h
#define __vtkStackLoadDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include <vtkSlicerStackLoadModuleMRMLDisplayableManagerExport.h>

/// \brief Displayable manager for showing transforms in slice (2D) views.
///
/// Displays transforms in slice viewers as glyphs, deformed grid, or
/// contour lines
///
class VTK_SLICER_STACKLOAD_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkStackLoadDisplayableManager
  : public vtkMRMLAbstractSliceViewDisplayableManager
{

public:

  static vtkStackLoadDisplayableManager* New();
  vtkTypeMacro(vtkStackLoadDisplayableManager, vtkMRMLAbstractSliceViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:

  vtkStackLoadDisplayableManager();
  ~vtkStackLoadDisplayableManager() override;

  void UnobserveMRMLScene() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;

  /// Update Actors based on transforms in the scene
  void UpdateFromMRML() override;

  void OnMRMLSceneStartClose() override;
  void OnMRMLSceneEndClose() override;

  void OnMRMLSceneEndBatchProcess() override;

  /// Initialize the displayable manager based on its associated
  /// vtkMRMLSliceNode
  void Create() override;

private:

  vtkStackLoadDisplayableManager(const vtkStackLoadDisplayableManager&) = delete;
  void operator=(const vtkStackLoadDisplayableManager&) = delete;

  class vtkInternal;
  vtkInternal * Internal;
  friend class vtkInternal;
};

#endif
