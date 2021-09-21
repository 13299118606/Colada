#ifndef __qSlicerGainWidget_p_h
#define __qSlicerGainWidget_p_h

// Slicer includes
#include "qSlicerGainWidget.h"
#include <qScienceSpinBox.h>

// ctk includes
#include <ctkDoubleSlider.h>

class Q_SLICER_MODULE_ZOOM_WIDGETS_EXPORT qSlicerGainWidgetPrivate {
  Q_DECLARE_PUBLIC(qSlicerGainWidget);

protected:
  qSlicerGainWidget *const q_ptr;

public:
  typedef qSlicerGainWidgetPrivate Superclass;
  qSlicerGainWidgetPrivate(qSlicerGainWidget &q);
  virtual ~qSlicerGainWidgetPrivate();

  virtual void init();
  virtual void setupUi();

protected:
  ctkDoubleSlider *slider;
  qScienceSpinBox *spinBox;
  double fakeGain, realGain;
  int numberOfSliderSteps;
};

#endif
