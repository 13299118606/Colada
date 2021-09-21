#ifndef __qScienceSpinBoxDelegate_p_h
#define __qScienceSpinBoxDelegate_p_h

// Colada includes
#include "qScienceSpinBoxDelegate.h"

class Q_COLADA_APP_EXPORT qScienceSpinBoxDelegatePrivate {
  Q_DECLARE_PUBLIC(qScienceSpinBoxDelegate);

protected:
  qScienceSpinBoxDelegate *const q_ptr;

public:
  typedef qScienceSpinBoxDelegatePrivate Superclass;
  qScienceSpinBoxDelegatePrivate(qScienceSpinBoxDelegate &q);
  virtual ~qScienceSpinBoxDelegatePrivate();

  virtual void init(int min, int max, int step, QString prefix, QString suffix);

private:
  QString prefix, suffix;
  int min, max, step;
};

#endif