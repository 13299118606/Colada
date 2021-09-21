#ifndef __qSpinBoxDelegate_p_h
#define __qSpinBoxDelegate_p_h

// Colada includes
#include "qSpinBoxDelegate.h"

class Q_COLADA_APP_EXPORT qSpinBoxDelegatePrivate {
  Q_DECLARE_PUBLIC(qSpinBoxDelegate);

protected:
  qSpinBoxDelegate *const q_ptr;

public:
  typedef qSpinBoxDelegatePrivate Superclass;
  qSpinBoxDelegatePrivate(qSpinBoxDelegate &q);
  virtual ~qSpinBoxDelegatePrivate();

  virtual void init(int min, int max, int step, QString prefix, QString suffix);

private:
  QString prefix, suffix;
  int min, max, step;
};

#endif