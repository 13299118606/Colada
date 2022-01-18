#ifndef __qH5SurveyTypeComboBox_p_h
#define __qH5SurveyTypeComboBox_p_h

#include "qH5SurveyTypeComboBox.h"

class QLineEdit;
class QHBoxLayout;
class QGridLayout;

class Q_COLADA_APP_EXPORT qH5SurveyTypeComboBoxPrivate {
  Q_DECLARE_PUBLIC(qH5SurveyTypeComboBox);

protected:
  qH5SurveyTypeComboBox *const q_ptr;

public:
  typedef qH5SurveyTypeComboBoxPrivate Superclass;
  qH5SurveyTypeComboBoxPrivate(qH5SurveyTypeComboBox &q);
  virtual ~qH5SurveyTypeComboBoxPrivate();

  virtual void init();
};

#endif
