#ifndef __qColadaH5MapTreeView_p_h
#define __qColadaH5MapTreeView_p_h

// Colada includes
#include "qColadaH5MapTreeView.h"
#include "qColadaH5TreeView_p.h"

class Q_COLADA_APP_EXPORT qColadaH5MapTreeViewPrivate
    : public qColadaH5TreeViewPrivate 
{
  Q_DECLARE_PUBLIC(qColadaH5MapTreeView);

public:
	typedef qColadaH5TreeViewPrivate Superclass;
  qColadaH5MapTreeViewPrivate(qColadaH5MapTreeView &q);
	virtual ~qColadaH5MapTreeViewPrivate();

  virtual void init();
};

#endif