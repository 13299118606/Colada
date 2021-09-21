#ifndef __qColadaH5WellTreeView_p_h
#define __qColadaH5WellTreeView_p_h

// Colada includes
#include "qColadaH5WellTreeView.h"
#include "qColadaH5TreeView_p.h"

class Q_COLADA_APP_EXPORT qColadaH5WellTreeViewPrivate
    : public qColadaH5TreeViewPrivate {
  Q_DECLARE_PUBLIC(qColadaH5WellTreeView);

public:
  typedef qColadaH5TreeViewPrivate Superclass;
  qColadaH5WellTreeViewPrivate(qColadaH5WellTreeView &q);
  virtual ~qColadaH5WellTreeViewPrivate();

  virtual void init();
};

#endif