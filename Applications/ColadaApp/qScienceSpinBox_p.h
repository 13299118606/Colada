#ifndef __qScienceSpinBox_p_h
#define __qScienceSpinBox_p_h

// Colada includes
#include "qScienceSpinBox.h"

// Qt includes
#include <QDoubleValidator>

class Q_COLADA_APP_EXPORT qScienceSpinBoxPrivate {
  Q_DECLARE_PUBLIC(qScienceSpinBox);

protected:
  qScienceSpinBox *const q_ptr;

public:
  typedef qScienceSpinBoxPrivate Superclass;
  qScienceSpinBoxPrivate(qScienceSpinBox &q);
  virtual ~qScienceSpinBoxPrivate();

  virtual void init(QWidget *parent);
  void initLocalValues(QWidget *parent);

protected:
  int dispDecimals;
  QChar delimiter, thousand;
  QDoubleValidator *v;
};

#endif