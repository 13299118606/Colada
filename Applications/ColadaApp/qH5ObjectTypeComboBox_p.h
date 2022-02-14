#ifndef __qH5ObjectTypeComboBox_p_h
#define __qH5ObjectTypeComboBox_p_h

#include "qH5ObjectTypeComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5ObjectTypeComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5ObjectTypeComboBox);

protected:
  qH5ObjectTypeComboBox *const q_ptr;

public:
  typedef qH5ObjectTypeComboBoxPrivate Superclass;
  qH5ObjectTypeComboBoxPrivate(qH5ObjectTypeComboBox &q);
  virtual ~qH5ObjectTypeComboBoxPrivate();

  virtual void init();
};

#endif
