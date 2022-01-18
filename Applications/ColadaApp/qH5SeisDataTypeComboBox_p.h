#ifndef __qH5SeisDataTypeComboBox_p_h
#define __qH5SeisDataTypeComboBox_p_h

#include "qH5SeisDataTypeComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5SeisDataTypeComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5SeisDataTypeComboBox);

protected:
  qH5SeisDataTypeComboBox *const q_ptr;

public:
  typedef qH5SeisDataTypeComboBoxPrivate Superclass;
  qH5SeisDataTypeComboBoxPrivate(qH5SeisDataTypeComboBox &q);
  virtual ~qH5SeisDataTypeComboBoxPrivate();

  virtual void init();
};

#endif
