#ifndef __qLineEditDelegate_p_h
#define __qLineEditDelegate_p_h

// Colada includes
#include "qLineEditDelegate.h"

class Q_COLADA_APP_EXPORT qLineEditDelegatePrivate {
  Q_DECLARE_PUBLIC(qLineEditDelegate);

protected:
  qLineEditDelegate *const q_ptr;

public:
  typedef qLineEditDelegatePrivate Superclass;
  qLineEditDelegatePrivate(qLineEditDelegate &q);
  virtual ~qLineEditDelegatePrivate();

  virtual void init(QString text, QValidator *validator);

private:
  QString text;
  QValidator *validator;
};

#endif