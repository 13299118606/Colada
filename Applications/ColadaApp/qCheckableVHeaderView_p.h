#ifndef __qCheckableVHeaderView_p_h
#define __qCheckableVHeaderView_p_h

// Colada includes
#include "qCheckableVHeaderView.h"

class Q_COLADA_APP_EXPORT qCheckableVHeaderViewPrivate {
  Q_DECLARE_PUBLIC(qCheckableVHeaderView);

protected:
  qCheckableVHeaderView *const q_ptr;

public:
  typedef qCheckableVHeaderViewPrivate Superclass;
  qCheckableVHeaderViewPrivate(qCheckableVHeaderView &q);
  virtual ~qCheckableVHeaderViewPrivate();

  virtual void init();
};

#endif