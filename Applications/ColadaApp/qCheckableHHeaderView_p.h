#ifndef __qCheckableHHeaderView_p_h
#define __qCheckableHHeaderView_p_h

// Colada includes
#include "qCheckableHHeaderView.h"

class Q_COLADA_APP_EXPORT qCheckableHHeaderViewPrivate {
  Q_DECLARE_PUBLIC(qCheckableHHeaderView);

protected:
  qCheckableHHeaderView *const q_ptr;

public:
  typedef qCheckableHHeaderViewPrivate Superclass;
  qCheckableHHeaderViewPrivate(qCheckableHHeaderView &q);
  virtual ~qCheckableHHeaderViewPrivate();

  virtual void init();
};

#endif