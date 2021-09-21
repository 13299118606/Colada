// Colada includes
#include "qSpinBoxDelegate.h"
#include "qSpinBoxDelegate_p.h"

// Qt includes
#include <QLineEdit>
#include <QSpinBox>

qSpinBoxDelegatePrivate::qSpinBoxDelegatePrivate(qSpinBoxDelegate &q)
    : q_ptr(&q) {}

qSpinBoxDelegatePrivate::~qSpinBoxDelegatePrivate() {}

void qSpinBoxDelegatePrivate::init(int min, int max, int step, QString prefix,
                                   QString suffix) {
  Q_Q(qSpinBoxDelegate);
  this->min = min;
  this->max = max;
  this->step = step;
  this->prefix = prefix;
  this->suffix = suffix;
}

qSpinBoxDelegate::qSpinBoxDelegate(int min, int max, int step, QString prefix,
                                 QString suffix, QObject *parent)
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