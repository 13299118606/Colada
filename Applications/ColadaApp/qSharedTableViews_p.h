#ifndef __qSharedTableViews_p_h
#define __qSharedTableViews_p_h

#include "qSharedTableViews.h"

class QTableView;
class QToolButton;
class QGridLayout;

class Q_COLADA_APP_EXPORT qSharedTableViewsPrivate {
  Q_DECLARE_PUBLIC(qSharedTableViews);

protected:
  qSharedTableViews *const q_ptr;

public:
  typedef qSharedTableViewsPrivate Superclass;
  qSharedTableViewsPrivate(qSharedTableViews &q);
  virtual ~qSharedTableViewsPrivate();

  virtual void init();

  QTableView* firstTableView, *secondTableView;
  QToolButton* addToolBtn, *removeToolBtn;
  QGridLayout* gridLayout;
};

#endif
