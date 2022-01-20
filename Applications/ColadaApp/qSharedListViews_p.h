#ifndef __qSharedListViews_p_h
#define __qSharedListViews_p_h

#include "qSharedListViews.h"

class QListView;
class QToolButton;
class QGridLayout;

class Q_COLADA_APP_EXPORT qSharedListViewsPrivate {
  Q_DECLARE_PUBLIC(qSharedListViews);

protected:
  qSharedListViews *const q_ptr;

public:
  typedef qSharedListViewsPrivate Superclass;
  qSharedListViewsPrivate(qSharedListViews &q);
  virtual ~qSharedListViewsPrivate();

  virtual void init();

  QListView* firstListView, *secondListView;
  QToolButton* addToolBtn, *removeToolBtn;
  QGridLayout* gridLayout;
};

#endif
