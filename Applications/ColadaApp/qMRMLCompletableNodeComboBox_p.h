#ifndef __qMRMLCompletableNodeComboBox_p_h
#define __qMRMLCompletableNodeComboBox_p_h

// qMRML includes
#include "qMRMLCompletableNodeComboBox.h"
#include "qMRMLNodeComboBox_p.h"

class Q_COLADA_APP_EXPORT qMRMLCompletableNodeComboBoxPrivate
    : public qMRMLNodeComboBoxPrivate {
  Q_DECLARE_PUBLIC(qMRMLCompletableNodeComboBox);

public:
  typedef qMRMLNodeComboBoxPrivate Superclass;
  qMRMLCompletableNodeComboBoxPrivate(qMRMLCompletableNodeComboBox &q);
  virtual ~qMRMLCompletableNodeComboBoxPrivate();

  virtual void init();
};

#endif
