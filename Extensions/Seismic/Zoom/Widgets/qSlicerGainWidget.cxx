// Slicer includes
#include "qSlicerGainWidget.h"
#include "qSlicerGainWidget_p.h"
#include "qScienceSpinBox.h"

// Qt includes
#include <QLayout>
#include <ctkDoubleSlider.h>

qSlicerGainWidgetPrivate::qSlicerGainWidgetPrivate(qSlicerGainWidget &q)
    : q_ptr(&q) {}

qSlicerGainWidgetPrivate::~qSlicerGainWidgetPrivate() {}

void qSlicerGainWidgetPrivate::init() {
  Q_Q(qSlicerGainWidget);
  this->setupUi();
}

void qSlicerGainWidgetPrivate::setupUi() {
  Q_Q(qSlicerGainWidget);
  slider = new ctkDoubleSlider;
  spinBox = new qScienceSpinBox;

  double boundVal = 100;
  fakeGain = 1;
  realGain = 1;
  numberOfSliderSteps = 201;

  slider->setPageStep(0);
  slider->setOrientation(Qt::Horizontal);
  slider->setMinimum(-boundVal);
  slider->setMaximum(boundVal);
  slider->setValue(fakeGain);
//  slider->setSizePolicy(QSizePolicy::MinimumExpanding,
//                        QSizePolicy::MinimumExpanding);

  spinBox->setDecimals(2);
  spinBox->setMinimum(-qInf());
  spinBox->setMaximum(qInf());
  spinBox->setValue(fakeGain);
  spinBox->setSingleStep(1);
  spinBox->setMaximumWidth(70 + spinBox->decimals()*5);
  spinBox->adjustSize();
//  spinBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

  QHBoxLayout *hLayout = new QHBoxLayout(q);
  hLayout->addWidget(slider);
  hLayout->addWidget(spinBox);
  hLayout->setStretch(0, 0);
  hLayout->setStretch(1, 1);

  QObject::connect(slider, &ctkDoubleSlider::sliderMoved, q,
                   &qSlicerGainWidget::onSliderMoved);
  QObject::connect(slider, &ctkDoubleSlider::sliderReleased, q,
                   &qSlicerGainWidget::onSliderReleased);
  QObject::connect(spinBox, &qScienceSpinBox::editingFinished, q,
                   &qSlicerGainWidget::onSpinboxEditingFinished);
}

qSlicerGainWidget::qSlicerGainWidget(QWidget *parent)
    : QWidget(parent), d_ptr(new qSlicerGainWidgetPrivate(*this)) {
  Q_D(qSlicerGainWidget);
  d->init();
}

qSlicerGainWidget::qSlicerGainWidget(qSlicerGainWidgetPrivate *pimpl,
                                     QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qSlicerGainWidget::~qSlicerGainWidget() {}

void qSlicerGainWidget::setGain(double val) {
  Q_D(qSlicerGainWidget);
  d->spinBox->setValue(val);
  onSpinboxEditingFinished();
}

double qSlicerGainWidget::getGain() {
  Q_D(qSlicerGainWidget);
  return d->spinBox->value();
}

ctkDoubleSlider *qSlicerGainWidget::getSlider() {
  Q_D(qSlicerGainWidget);
  return d->slider;
}

qScienceSpinBox *qSlicerGainWidget::getSpinbox() {
  Q_D(qSlicerGainWidget);
  return d->spinBox;
}

void qSlicerGainWidget::setMinimum(double val){
  Q_D(qSlicerGainWidget);
  d->spinBox->setMinimum(val);
}

void qSlicerGainWidget::setMaximum(double val){
  Q_D(qSlicerGainWidget);
  d->spinBox->setMaximum(val);
}

void qSlicerGainWidget::setNumberOfSliderSteps(int val) {
  Q_D(qSlicerGainWidget);
  d->numberOfSliderSteps = val;
  d->slider->setSingleStep((d->slider->maximum() - d->slider->minimum()) / val);
}

void qSlicerGainWidget::onSliderMoved(double val) {
  Q_D(qSlicerGainWidget);
  double oldVal = d->spinBox->value();
  double meanVal = (d->slider->maximum() + d->slider->minimum()) / 2;
  d->spinBox->blockSignals(true);
  if (val >= meanVal) {
    d->realGain = d->fakeGain * (0.01 * (val - meanVal) + 1);
    d->spinBox->setValue(d->realGain);
  } else if (val < meanVal) {
    d->realGain = d->fakeGain * (0.005 * (val - meanVal) + 1);
    d->spinBox->setValue(d->realGain);
  }
  d->spinBox->blockSignals(false);

  emit gainChanged(d->spinBox->value());
  emit gainChanged(oldVal, d->spinBox->value());
}

void qSlicerGainWidget::onSpinboxEditingFinished() {
  Q_D(qSlicerGainWidget);
  double oldVal = d->realGain;
  d->realGain = d->spinBox->value();
  d->fakeGain = d->spinBox->value();

  emit gainChanged(d->spinBox->value());
  emit gainChanged(oldVal, d->spinBox->value());
}

void qSlicerGainWidget::onSliderReleased() {
  Q_D(qSlicerGainWidget);
  d->fakeGain = d->spinBox->value();

  d->slider->setMinimum(d->slider->value() - 100);
  d->slider->setMaximum(d->slider->value() + 100);
}
