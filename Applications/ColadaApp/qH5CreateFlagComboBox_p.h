#ifndef __qH5CreateFlagComboBox_p_h
#define __qH5CreateFlagComboBox_p_h

#include "qH5CreateFlagComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5CreateFlagComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5CreateFlagComboBox);

protected:
  qH5CreateFlagComboBox *const q_ptr;

public:
  typedef qH5CreateFlagComboBoxPrivate Superclass;
  qH5CreateFlagComboBoxPrivate(qH5CreateFlagComboBox &q);
  virtual ~qH5CreateFlagComboBoxPrivate();

  virtual void init();
};

#endif
