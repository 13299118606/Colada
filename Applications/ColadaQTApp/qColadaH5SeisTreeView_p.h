#ifndef __qColadaH5SeisTreeView_p_h
#define __qColadaH5SeisTreeView_p_h

// Colada includes
#include "qColadaH5SeisTreeView.h"
#include "qColadaH5TreeView_p.h"

class Q_COLADA_APP_EXPORT qColadaH5SeisTreeViewPrivate
    : public qColadaH5TreeViewPrivate {
  Q_DECLARE_PUBLIC(qColadaH5SeisTreeView);

public:
  typedef qColadaH5TreeViewPrivate Superclass;
  qColadaH5SeisTreeViewPrivate(qColadaH5SeisTreeView &q);
  virtual ~qColadaH5SeisTreeViewPrivate();

  virtual void init();
};

#endif