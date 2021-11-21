#ifndef __qCRSDropTableView_p_h
#define __qCRSDropTableView_p_h

#include "qH5ItemDropLineEdit.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5ItemDropLineEditPrivate {
  Q_DECLARE_PUBLIC(qH5ItemDropLineEdit);

protected:
  qH5ItemDropLineEdit *const q_ptr;

public:
  typedef qH5ItemDropLineEditPrivate Superclass;
  qH5ItemDropLineEditPrivate(qH5ItemDropLineEdit &q);
  virtual ~qH5ItemDropLineEditPrivate();

  virtual void init();

protected:
  QLineEdit* containerLineEdit, *objectLineEdit;
  QGridLayout* gridLayout;
};

#endif
