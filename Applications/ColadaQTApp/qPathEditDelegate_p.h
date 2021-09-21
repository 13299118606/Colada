#ifndef __qPathEditDelegate_p_h
#define __qPathEditDelegate_p_h

// Colada includes
#include "qPathEditDelegate.h"

class Q_COLADA_APP_EXPORT qPathEditDelegatePrivate {
  Q_DECLARE_PUBLIC(qPathEditDelegate);

protected:
  qPathEditDelegate *const q_ptr;

public:
  typedef qPathEditDelegatePrivate Superclass;
  qPathEditDelegatePrivate(qPathEditDelegate &q);
  virtual ~qPathEditDelegatePrivate();

  virtual void init(QString label, QStringList nameFilters, ctkPathLineEdit::Filters filters);

private:
  QString label;
  QStringList nameFilters;
  ctkPathLineEdit::Filters filters;
};

#endif