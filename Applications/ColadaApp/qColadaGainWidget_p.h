#ifndef __qColadaGainWidget_p_h
#define __qColadaGainWidget_p_h

// Colada includes
#include "qColadaGainWidget.h"
#include <qScienceSpinBox.h>

// ctk includes
#include <ctkDoubleSlider.h>

class Q_COLADA_APP_EXPORT qColadaGainWidgetPrivate {
  Q_DECLARE_PUBLIC(qColadaGainWidget);

protected:
  qColadaGainWidget *const q_ptr;

public:
  typedef qColadaGainWidgetPrivate Superclass;
  qColadaGainWidgetPrivate(qColadaGainWidget &q);
  virtual ~qColadaGainWidgetPrivate();

  virtual void init();
  virtual void setupUi();

protected:
  ctkDoubleSlider *slider;
  qScienceSpinBox *spinBox;
  double gain;
  int numberOfSliderSteps;
};

#endif