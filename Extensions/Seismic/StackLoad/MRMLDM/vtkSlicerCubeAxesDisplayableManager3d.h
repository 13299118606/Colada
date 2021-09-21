#ifndef __vtkSlicerCubeAxesDisplayableManager3d_h
#define __vtkSlicerCubeAxesDisplayableManager3d_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractThreeDViewDisplayableManager.h"
#include <vtkSlicerStackLoadModuleMRMLDisplayableManagerExport.h>

// Qt declaration
class QColor;

/// \brief Displayable manager that displays 2D WCS axes in 2D view
class VTK_SLICER_STACKLOAD_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkSlicerCubeAxesDisplayableManager3d
  : public vtkMRMLAbstractThreeDViewDisplayableManager
{

public:
  static vtkSlicerCubeAxesDisplayableManager3d* New();
  vtkTypeMacro(vtkSlicerCubeAxesDisplayableManager3d,vtkMRMLAbstractThreeDViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void showAxes(bool val);

protected:

  vtkSlicerCubeAxesDisplayableManager3d();
  virtual ~vtkSlicerCubeAxesDisplayableManager3d();

  /// Observe the View node and initialize the renderer accordingly.
  virtual void Create() override;

  /// Called each time the view node is modified.
  /// Internally update the renderer from the view node
  /// \sa UpdateFromMRMLViewNode()
  virtual void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller) override;

private:

  vtkSlicerCubeAxesDisplayableManager3d(const vtkSlicerCubeAxesDisplayableManager3d&);// Not implemented
  void operator=(const vtkSlicerCubeAxesDisplayableManager3d&); // Not Implemented

  class vtkInternal;
  vtkInternal * Internal;
};

#endif
