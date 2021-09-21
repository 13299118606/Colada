#ifndef __qComboBoxDelegate_p_h
#define __qComboBoxDelegate_p_h

// Colada includes
#include "qComboBoxDelegate.h"

class Q_COLADA_APP_EXPORT qComboBoxDelegatePrivate {
  Q_DECLARE_PUBLIC(qComboBoxDelegate);

protected:
  qComboBoxDelegate *const q_ptr;

public:
  typedef qComboBoxDelegatePrivate Superclass;
  qComboBoxDelegatePrivate(qComboBoxDelegate &q);
  virtual ~qComboBoxDelegatePrivate();

  virtual void init(QStringList texts);

public:
  QStringList texts;
};

#endif