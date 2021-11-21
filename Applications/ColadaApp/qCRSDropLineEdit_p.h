#ifndef __qCRSDropLineEdit_p_h
#define __qCRSDropLineEdit_p_h

#include "qCRSDropLineEdit.h"

class QLineEdit;
class QHBoxLayout;

class Q_COLADA_APP_EXPORT qCRSDropLineEditPrivate {
  Q_DECLARE_PUBLIC(qCRSDropLineEdit);

protected:
  qCRSDropLineEdit *const q_ptr;

public:
  typedef qCRSDropLineEditPrivate Superclass;
  qCRSDropLineEditPrivate(qCRSDropLineEdit &q);
  virtual ~qCRSDropLineEditPrivate();

  virtual void init();

protected:
  QLineEdit* NameLineEdit, *AuthNameLineEdit, *CodeLineEdit;
  QHBoxLayout* HLayout;
};

#endif
