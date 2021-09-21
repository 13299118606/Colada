#ifndef __vtkSlicerStackLoadDisplayableManager_h
#define __vtkSlicerStackLoadDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include <vtkSlicerStackLoadModuleMRMLDisplayableManagerExport.h>

// Qt declaration
class QColor;

/// \brief Displayable manager that displays 2D WCS axes in 2D view
class VTK_SLICER_STACKLOAD_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkSlicerStackLoadDisplayableManager
  : public vtkMRMLAbstractSliceViewDisplayableManager
{
  friend class vtkStackLoadRendererUpdateObserver;

public:
  static vtkSlicerStackLoadDisplayableManager* New();
  vtkTypeMacro(vtkSlicerStackLoadDisplayableManager,vtkMRMLAbstractSliceViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /// Get vtkMarkerRenderer
  vtkRenderer* vtkMarkerRenderer();

  /// Set annotation color
  void SetAnnotationsColor(double red,
                           double green,
                           double blue);

  /// Set font style
  void SetAnnotationsFontStyle(const char* font);

  /// Set font size
  void SetAnnotationsFontSize(int size);

protected:

  vtkSlicerStackLoadDisplayableManager();
  virtual ~vtkSlicerStackLoadDisplayableManager();

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

private:

  vtkSlicerStackLoadDisplayableManager(const vtkSlicerStackLoadDisplayableManager&);// Not implemented
  void operator=(const vtkSlicerStackLoadDisplayableManager&); // Not Implemented

  class vtkInternal;
  vtkInternal * Internal;
};

#endif
