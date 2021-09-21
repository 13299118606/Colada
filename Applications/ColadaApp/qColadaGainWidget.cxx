// Colada includes
#include "qColadaGainWidget.h"
#include "qColadaGainWidget_p.h"
#include "qScienceSpinBox.h"

// Qt includes
#include <QLayout>
#include <ctkDoubleSlider.h>

qColadaGainWidgetPrivate::qColadaGainWidgetPrivate(qColadaGainWidget &q)
    : q_ptr(&q) {}

qColadaGainWidgetPrivate::~qColadaGainWidgetPrivate() {}

void qColadaGainWidgetPrivate::init() {
  Q_Q(qColadaGainWidget);
  this->setupUi();
}

void qColadaGainWidgetPrivate::setupUi() {
  Q_Q(qColadaGainWidget);
  slider = new ctkDoubleSlider;
  spinBox = new qScienceSpinBox;

  double boundVal = 100;
  gain = 1;
  numberOfSliderSteps = 201;

  slider->setPageStep(0);
  slider->setOrientation(Qt::Horizontal);
  slider->setMinimum(-boundVal);
  slider->setMaximum(boundVal);
  slider->setValue(gain);
  slider->setSizePolicy(QSizePolicy::MinimumExpanding,
                        QSizePolicy::MinimumExpanding);

  spinBox->setDecimals(2);
  spinBox->setMinimum(-qInf());
  spinBox->setMaximum(qInf());
  spinBox->setValue(gain);
  spinBox->setSingleStep(1);
  spinBox->adjustSize();
  spinBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

  QHBoxLayout *hLayout = new QHBoxLayout(q);
  hLayout->addWidget(slider);
  hLayout->addWidget(spinBox);
  hLayout->setStretch(0, 1);
  hLayout->setStretch(1, 0);

  QObject::connect(slider, &ctkDoubleSlider::sliderMoved, q,
                   &qColadaGainWidget::onSliderMoved);
  QObject::connect(slider, &ctkDoubleSlider::sliderReleased, q,
                   &qColadaGainWidget::onSliderReleased);
  QObject::connect(spinBox, &qScienceSpinBox::editingFinished, q,
                   &qColadaGainWidget::onSpinboxEditingFinished);
}

qColadaGainWidget::qColadaGainWidget(QWidget *parent)
    : QWidget(parent), d_ptr(new qColadaGainWidgetPrivate(*this)) {
  Q_D(qColadaGainWidget);
  d->init();
}

qColadaGainWidget::qColadaGainWidget(qColadaGainWidgetPrivate *pimpl,
                                     QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qColadaGainWidget::~qColadaGainWidget() {}

void qColadaGainWidget::setGain(double val) {
  Q_D(qColadaGainWidget);
  d->spinBox->setValue(val);
  onSpinboxEditingFinished();
}

double qColadaGainWidget::getGain() {
  Q_D(qColadaGainWidget);
  return d->gain;
}

ctkDoubleSlider *qColadaGainWidget::getSlicer() {
  Q_D(qColadaGainWidget);
  return d->slider;
}

qScienceSpinBox *qColadaGainWidget::getSpinbox() {
  Q_D(qColadaGainWidget);
  return d->spinBox;
}

void qColadaGainWidget::setNumberOfSliderSteps(int val) {
  Q_D(qColadaGainWidget);
  d->numberOfSliderSteps = val;
  d->slider->setSingleStep((d->slider->maximum() - d->slider->minimum()) / val);
}

void qColadaGainWidget::onSliderMoved(double val) {
  Q_D(qColadaGainWidget);
  double meanVal = (d->slider->maximum() + d->slider->minimum()) / 2;
  d->spinBox->blockSignals(true);
  if (val >= meanVal) {
    d->spinBox->setValue(d->gain * (0.01 * (val - meanVal) + 1));
  } else if (val < meanVal) {
    d->spinBox->setValue(d->gain * (0.005 * (val - meanVal) + 1));
  }
  d->spinBox->blockSignals(false);

  emit gainChanged(d->spinBox->value());
}

void qColadaGainWidget::onSpinboxEditingFinished() {
  Q_D(qColadaGainWidget);
  d->slider->blockSignals(true);
  d->gain = d->spinBox->value();
  d->slider->blockSignals(false);

  emit gainChanged(d->spinBox->value());
}

void qColadaGainWidget::onSliderReleased() {
  Q_D(qColadaGainWidget);
  d->gain = d->spinBox->value();

  d->slider->setMinimum(d->slider->value() - 100);
  d->slider->setMaximum(d->slider->value() + 100);
}
