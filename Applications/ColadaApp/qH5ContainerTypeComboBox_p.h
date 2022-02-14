#ifndef __qH5ContainerTypeComboBox_p_h
#define __qH5ContainerTypeComboBox_p_h

#include "qH5ContainerTypeComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5ContainerTypeComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5ContainerTypeComboBox);

protected:
  qH5ContainerTypeComboBox *const q_ptr;

public:
  typedef qH5ContainerTypeComboBoxPrivate Superclass;
  qH5ContainerTypeComboBoxPrivate(qH5ContainerTypeComboBox &q);
  virtual ~qH5ContainerTypeComboBoxPrivate();

  virtual void init();
};

#endif
