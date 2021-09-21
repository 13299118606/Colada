#ifndef __qColadaH5SurfTreeView_p_h
#define __qColadaH5SurfTreeView_p_h

// Colada includes
#include "qColadaH5SurfTreeView.h"
#include "qColadaH5TreeView_p.h"

class Q_COLADA_APP_EXPORT qColadaH5SurfTreeViewPrivate
    : public qColadaH5TreeViewPrivate 
{
  Q_DECLARE_PUBLIC(qColadaH5SurfTreeView);

public:
	typedef qColadaH5TreeViewPrivate Superclass;
  qColadaH5SurfTreeViewPrivate(qColadaH5SurfTreeView &q);
	virtual ~qColadaH5SurfTreeViewPrivate();

  virtual void init();
};

#endif