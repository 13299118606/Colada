// Colada includes
#include "qLineEditDelegate.h"
#include "qLineEditDelegate_p.h"

// Qt includes
#include <QLineEdit>

qLineEditDelegatePrivate::qLineEditDelegatePrivate(qLineEditDelegate &q)
    : q_ptr(&q) {}

qLineEditDelegatePrivate::~qLineEditDelegatePrivate() {}

void qLineEditDelegatePrivate::init(const QString &text,
                                    QValidator *validator) {
  Q_Q(qLineEditDelegate);
  this->text = text;
  this->validator = validator;
}

qLineEditDelegate::qLineEditDelegate(QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qLineEditDelegatePrivate(*this)) {
  Q_D(qLineEditDelegate);
  d->init("", nullptr);
}

qLineEditDelegate::qLineEditDelegate(const QString &text, QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qLineEditDelegatePrivate(*this)) {
  Q_D(qLineEditDelegate);
  d->init(text, nullptr);
}

qLineEditDelegate::qLineEditDelegate(const QString &text, QValidator *validator,
                                     QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qLineEditDelegatePrivate(*this)) {
  Q_D(qLineEditDelegate);
  d->init(text, validator);
}

qLineEditDelegate::qLineEditDelegate(qLineEditDelegatePrivate *pimpl,
                                     QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qLineEditDelegate::~qLineEditDelegate() {}

void qLineEditDelegate::setText(const QString &text) {
  Q_D(qLineEditDelegate);
  d->text = text;
}

void qLineEditDelegate::setValidator(QValidator *validator) {
  Q_D(qLineEditDelegate);
  d->validator = validator;
}

QString qLineEditDelegate::getText() {
  Q_D(qLineEditDelegate);
  return d->text;
}

QValidator* qLineEditDelegate::getValidator() {
  Q_D(qLineEditDelegate);
  return d->validator;
}

QWidget *qLineEditDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  Q_D(const qLineEditDelegate);
  QLineEdit *editor = new QLineEdit(d->text, parent);
  if (d->validator) {
    editor->setValidator(d->validator);
  }

  return editor;
}

void qLineEditDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  QString value = index.model()->data(index, Qt::EditRole).toString();

  QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
  lineEdit->setText(value);
}

void qLineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
  QString value = lineEdit->text();

  model->setData(index, value, Qt::EditRole);
}

void qLineEditDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
