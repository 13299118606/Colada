#ifndef __qColadaH5TreeView_p_h
#define __qColadaH5TreeView_p_h

// Colada includes
#include "qColadaH5TreeView.h"

class qSlicerApplication;
class qColadaAppMainWindow;

class Q_COLADA_APP_EXPORT qColadaH5TreeViewPrivate
{
  Q_DECLARE_PUBLIC(qColadaH5TreeView);

protected:
  qColadaH5TreeView* const q_ptr;

public:
  typedef qColadaH5TreeViewPrivate Superclass;
  qColadaH5TreeViewPrivate(qColadaH5TreeView& q);
  virtual ~qColadaH5TreeViewPrivate();

  virtual void init();

  qSlicerApplication* app;
  qColadaAppMainWindow* mainW;
};

#endif
