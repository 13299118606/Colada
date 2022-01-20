#ifndef __qH5ItemDropLineEdit_p_h
#define __qH5ItemDropLineEdit_p_h

#include "qH5ItemDropLineEdit.h"

class ctkPathLineEdit;
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
  ctkPathLineEdit* containerPathLineEdit;
  QLineEdit *objectLineEdit;
  QGridLayout* gridLayout;
};

#endif
