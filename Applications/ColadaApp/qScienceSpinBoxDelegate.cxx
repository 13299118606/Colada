// Colada includes
#include "qScienceSpinBoxDelegate.h"
#include "qScienceSpinBoxDelegate_p.h"
#include "qScienceSpinBox.h"

// Qt includes
#include <QLineEdit>

qScienceSpinBoxDelegatePrivate::qScienceSpinBoxDelegatePrivate(qScienceSpinBoxDelegate &q)
    : q_ptr(&q) {}

qScienceSpinBoxDelegatePrivate::~qScienceSpinBoxDelegatePrivate() {}

void qScienceSpinBoxDelegatePrivate::init(double min, double max, double step,
                                          const QString &prefix,
                                          const QString &suffix) {
  Q_Q(qScienceSpinBoxDelegate);
  this->min = min;
  this->max = max;
  this->step = step;
  this->prefix = prefix;
  this->suffix = suffix;
}

qScienceSpinBoxDelegate::qScienceSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      d_ptr(new qScienceSpinBoxDelegatePrivate(*this)) {
  Q_D(qScienceSpinBoxDelegate);
  d->init(std::numeric_limits<double>::min(), std::numeric_limits<double>::max(), 1,
          "", "");
}

qScienceSpinBoxDelegate::qScienceSpinBoxDelegate(double min, double max, double step,
                                                 const QString &prefix,
                                                 const QString &suffix,
                                                 QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qScienceSpinBoxDelegatePrivate(*this)) {
  Q_D(qScienceSpinBoxDelegate);
  d->init(min, max, step, prefix, suffix);
}

qScienceSpinBoxDelegate::qScienceSpinBoxDelegate(qScienceSpinBoxDelegatePrivate *pimpl,
                                   QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qScienceSpinBoxDelegate::~qScienceSpinBoxDelegate() {}

void qScienceSpinBoxDelegate::setRange(double bottom, double top) {
  Q_D(qScienceSpinBoxDelegate);
  d->min = bottom;
  d->max = top;
}

void qScienceSpinBoxDelegate::setMinValue(double val) {
  Q_D(qScienceSpinBoxDelegate);
  d->min = val;
}

void qScienceSpinBoxDelegate::setMaxValue(double val) {
  Q_D(qScienceSpinBoxDelegate);
  d->max = val;
}

void qScienceSpinBoxDelegate::setStep(double val) {
  Q_D(qScienceSpinBoxDelegate);
  d->step = val;
}

void qScienceSpinBoxDelegate::setPrefix(const QString &prefix) {
  Q_D(qScienceSpinBoxDelegate);
  d->prefix = prefix;
}

void qScienceSpinBoxDelegate::setSuffix(const QString &suffix) {
  Q_D(qScienceSpinBoxDelegate);
  d->suffix = suffix;
}

double qScienceSpinBoxDelegate::getMinValue() {
  Q_D(qScienceSpinBoxDelegate);
  return d->min;
}

double qScienceSpinBoxDelegate::getMaxValue() {
  Q_D(qScienceSpinBoxDelegate);
  return d->max;
}

double qScienceSpinBoxDelegate::getStep() {
  Q_D(qScienceSpinBoxDelegate);
  return d->step;
}

QString qScienceSpinBoxDelegate::getPrefix() {
  Q_D(qScienceSpinBoxDelegate);
  return d->prefix;
}

QString qScienceSpinBoxDelegate::getSuffix() {
  Q_D(qScienceSpinBoxDelegate);
  return d->suffix;
}

QWidget *qScienceSpinBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  Q_D(const qScienceSpinBoxDelegate);
  qScienceSpinBox *editor = new qScienceSpinBox(parent);
  editor->setFrame(false);
  //    editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
  editor->setMinimum(d->min);
  editor->setMaximum(d->max);
  editor->setSingleStep(d->step);
  if (!d->prefix.isEmpty())
    editor->setPrefix(d->prefix);

  if (!d->suffix.isEmpty())
    editor->setSuffix(d->suffix);

  return editor;
}

void qScienceSpinBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  double value = index.model()->data(index, Qt::EditRole).toDouble();

  qScienceSpinBox *spinBox = static_cast<qScienceSpinBox *>(editor);
  spinBox->setValue(value);
}

void qScienceSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  qScienceSpinBox *spinBox = static_cast<qScienceSpinBox *>(editor);
  spinBox->interpretText();
  double value = spinBox->value();

  model->setData(index, value, Qt::EditRole);
}

void qScienceSpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}