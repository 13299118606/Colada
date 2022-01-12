#ifndef __vtkMRMLCaptionsDisplayableManager_h
#define __vtkMRMLCaptionsDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractDisplayableManager.h"
#include <vtkSlicerCaptionsModuleMRMLDisplayableManagerExport.h>

class vtkMRMLDisplayableNode;

/// \brief Displayable manager that displays orienatation marker in a slice or 3D view
class VTK_SLICER_CAPTIONS_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLCaptionsDisplayableManager
  : public vtkMRMLAbstractDisplayableManager
{
  friend class vtkRulerRendererUpdateObserver;

public:
  static vtkMRMLCaptionsDisplayableManager* New();
  vtkTypeMacro(vtkMRMLCaptionsDisplayableManager,vtkMRMLAbstractDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void SetAndObserveMRMLNode(vtkMRMLDisplayableNode* node);
  /// cation is visible text
  void SetCaption(vtkMRMLDisplayableNode* node);
  void SetCaptionVisibility(vtkMRMLDisplayableNode* node);
  void SetCaptionsVisibility(bool val);
  void SetFontSize(int val);
  void SetBold(bool val);
  void SetItalic(bool val);
  void SetShadow(bool val);
  void SetColor(const double* x);
  void SetColor(double r, double g, double b);
  void SetOrientation(double val);
  void SetOpacity(double val);
  void SetFrame(bool val);
  void SetFrameWidth(int val);
  void SetFrameColor(const double* x);
  void SetFrameColor(double r, double g, double b);
  void SetRelativePosition(vtkMRMLDisplayableNode* node);
  void SetRelativePosition(double x, double y, double z);

protected:

  vtkMRMLCaptionsDisplayableManager();
  ~vtkMRMLCaptionsDisplayableManager() override;

  /// Observe the View node and initialize the renderer accordingly.
  void Create() override;

  /// Called each time the view node is modified.
  /// Internally update the renderer from the view node.
  /// \sa UpdateFromMRMLViewNode()
  void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller) override;

  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;

  void ReleaseObservedNode(vtkMRMLDisplayableNode* node);

private:

  vtkMRMLCaptionsDisplayableManager(const vtkMRMLCaptionsDisplayableManager&) = delete;
  void operator=(const vtkMRMLCaptionsDisplayableManager&) = delete;

  class vtkInternal;
  vtkInternal * Internal;

  friend class vtkMRMLCaptionsDisplayableManagerObserver;
  friend class qSlicerCaptionsWidget;
};

#endif
