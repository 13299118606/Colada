#ifndef __qH5DomainComboBox_p_h
#define __qH5DomainComboBox_p_h

#include "qH5DomainComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5DomainComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5DomainComboBox);

protected:
  qH5DomainComboBox *const q_ptr;

public:
  typedef qH5DomainComboBoxPrivate Superclass;
  qH5DomainComboBoxPrivate(qH5DomainComboBox &q);
  virtual ~qH5DomainComboBoxPrivate();

  virtual void init();
};

#endif
