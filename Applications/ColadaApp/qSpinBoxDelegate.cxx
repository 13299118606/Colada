// Colada includes
#include "qSpinBoxDelegate.h"
#include "qSpinBoxDelegate_p.h"

// Qt includes
#include <QLineEdit>
#include <QSpinBox>

qSpinBoxDelegatePrivate::qSpinBoxDelegatePrivate(qSpinBoxDelegate &q)
    : q_ptr(&q) {}

qSpinBoxDelegatePrivate::~qSpinBoxDelegatePrivate() {}

void qSpinBoxDelegatePrivate::init(int min, int max, int step,
                                   const QString &prefix,
                                   const QString &suffix) {
  Q_Q(qSpinBoxDelegate);
  this->min = min;
  this->max = max;
  this->step = step;
  this->prefix = prefix;
  this->suffix = suffix;
}

qSpinBoxDelegate::qSpinBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qSpinBoxDelegatePrivate(*this)) {
  Q_D(qSpinBoxDelegate);
  d->init(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1,
          "", "");
}

qSpinBoxDelegate::qSpinBoxDelegate(int min, int max, int step,
                                   const QString &prefix, const QString &suffix,
                                   QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qSpinBoxDelegatePrivate(*this)) {
  Q_D(qSpinBoxDelegate);
  d->init(min, max, step, prefix, suffix);
}

qSpinBoxDelegate::qSpinBoxDelegate(qSpinBoxDelegatePrivate* pimpl, QObject* parent)
  : QStyledItemDelegate(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qSpinBoxDelegate::~qSpinBoxDelegate() {}

void qSpinBoxDelegate::setRange(int bottom, int top) {
  Q_D(qSpinBoxDelegate);
  d->min = bottom;
  d->max = top;
}

void qSpinBoxDelegate::setMinValue(int val) {
  Q_D(qSpinBoxDelegate);
  d->min = val;
}

void qSpinBoxDelegate::setMaxValue(int val) {
  Q_D(qSpinBoxDelegate);
  d->max = val;
}

void qSpinBoxDelegate::setStep(int val) {
  Q_D(qSpinBoxDelegate);
  d->step = val;
}

void qSpinBoxDelegate::setPrefix(const QString &prefix) {
  Q_D(qSpinBoxDelegate);
  d->prefix = prefix;
}

void qSpinBoxDelegate::setSuffix(const QString &suffix) {
  Q_D(qSpinBoxDelegate);
  d->suffix = suffix;
}

int qSpinBoxDelegate::getMinValue() {
  Q_D(qSpinBoxDelegate);
  return d->min;
}

int qSpinBoxDelegate::getMaxValue() {
  Q_D(qSpinBoxDelegate);
  return d->max;
}

int qSpinBoxDelegate::getStep() {
  Q_D(qSpinBoxDelegate);
  return d->step;
}

QString qSpinBoxDelegate::getPrefix() {
  Q_D(qSpinBoxDelegate);
  return d->prefix;
}

QString qSpinBoxDelegate::getSuffix() {
  Q_D(qSpinBoxDelegate);
  return d->suffix;
}

QWidget *qSpinBoxDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
  Q_D(const qSpinBoxDelegate);
  QSpinBox *editor = new QSpinBox(parent);
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

void qSpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const {
  double value = index.model()->data(index, Qt::EditRole).toDouble();

  QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
  spinBox->setValue(value);
}

void qSpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
  QSpinBox *spinBox = static_cast<QSpinBox *>(editor);
  spinBox->interpretText();
  double value = spinBox->value();

  model->setData(index, value, Qt::EditRole);
}

void qSpinBoxDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}