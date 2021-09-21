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

  virtual void init(double min, double max, double step, const QString &prefix,
                    const QString &suffix);

private:
  QString prefix, suffix;
  double min, max, step;
};

#endif