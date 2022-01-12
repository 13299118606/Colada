#ifndef __vtkMRMLAxesDisplayableManager3D_h
#define __vtkMRMLAxesDisplayableManager3D_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractThreeDViewDisplayableManager.h"
#include <vtkSlicerAxesModuleMRMLDisplayableManagerExport.h>

class vtkCubeAxesActor;
class vtkMRMLDisplayableNode;

/// \brief Displayable manager that displays 2D cube axes in 2D view
class VTK_SLICER_AXES_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLAxesDisplayableManager3D
  : public vtkMRMLAbstractThreeDViewDisplayableManager
{

public:
  static vtkMRMLAxesDisplayableManager3D* New();
  vtkTypeMacro(vtkMRMLAxesDisplayableManager3D,vtkMRMLAbstractThreeDViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void SetAndObserveMRMLNodeBounds(vtkMRMLDisplayableNode* node);

  vtkCubeAxesActor* GetCubeAxesActor();
  vtkMRMLDisplayableNode* GetObservedNode();

protected:
  vtkMRMLAxesDisplayableManager3D();
  virtual ~vtkMRMLAxesDisplayableManager3D();

  /// Observe the View node and initialize the renderer accordingly.
  virtual void Create() override;

  /// Called each time the view node is modified.
  /// Internally update the renderer from the view node
  /// \sa UpdateFromMRMLViewNode()
  virtual void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller) override;

  /// Update the renderer from the view node properties.
  void UpdateFromViewNode();

  /// Update the renderer based on the master renderer (the one that the orientation marker follows)
  void UpdateFromRenderer();

  /// Set observed node to `nullptr`
  void ReleaseObservedNode();

private:

  vtkMRMLAxesDisplayableManager3D(const vtkMRMLAxesDisplayableManager3D&);// Not implemented
  void operator=(const vtkMRMLAxesDisplayableManager3D&); // Not Implemented

  class vtkInternal;
  vtkInternal * Internal;
  friend class vtkMRMLAxesDisplayableManager3DObserver;
  friend class qSlicerAxesModule;
  friend class qSlicerAxesModuleWidget;
  friend class qSlicerAxes3DWidget;
};

#endif
