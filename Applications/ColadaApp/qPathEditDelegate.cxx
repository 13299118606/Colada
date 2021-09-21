// Colada includes
#include "qPathEditDelegate.h"
#include "qPathEditDelegate_p.h"

qPathEditDelegatePrivate::qPathEditDelegatePrivate(qPathEditDelegate &q)
    : q_ptr(&q) {}

qPathEditDelegatePrivate::~qPathEditDelegatePrivate() {}

void qPathEditDelegatePrivate::init(const QString &label, const QStringList &nameFilters,
                                    ctkPathLineEdit::Filters filters) {
  Q_Q(qPathEditDelegate);
  this->label = label;
  this->nameFilters = nameFilters;
  this->filters = filters;
}

qPathEditDelegate::qPathEditDelegate(QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qPathEditDelegatePrivate(*this)) {
  Q_D(qPathEditDelegate);
  d->init("", QStringList(), ctkPathLineEdit::AllEntries);
}

qPathEditDelegate::qPathEditDelegate(const QString &label,
                                     ctkPathLineEdit::Filters filters,
                                     QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qPathEditDelegatePrivate(*this)) {
  Q_D(qPathEditDelegate);
  d->init(label, QStringList(), filters);
}

qPathEditDelegate::qPathEditDelegate(const QString &label,
                                     const QStringList &nameFilters,
                                     ctkPathLineEdit::Filters filters,
                                     QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qPathEditDelegatePrivate(*this)) {
  Q_D(qPathEditDelegate);
  d->init(label, nameFilters, filters);
}

qPathEditDelegate::qPathEditDelegate(qPathEditDelegatePrivate *pimpl,
                                     QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qPathEditDelegate::~qPathEditDelegate() {}

void qPathEditDelegate::setLabel(const QString &label) {
  Q_D(qPathEditDelegate);
  d->label = label;
}

void qPathEditDelegate::setNameFilters(const QStringList &nameFilters) {
  Q_D(qPathEditDelegate);
  d->nameFilters = nameFilters;
}

void qPathEditDelegate::setFilters(ctkPathLineEdit::Filters filters) {
  Q_D(qPathEditDelegate);
  d->filters = filters;
}

QString qPathEditDelegate::getLabel() {
  Q_D(qPathEditDelegate);
  return d->label;
}

QStringList qPathEditDelegate::getNameFilters() {
  Q_D(qPathEditDelegate);
  return d->nameFilters;
}

ctkPathLineEdit::Filters qPathEditDelegate::getFilters() {
  Q_D(qPathEditDelegate);
  return d->filters;
}

QWidget *qPathEditDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
  Q_D(const qPathEditDelegate);
  ctkPathLineEdit *editor =
      new ctkPathLineEdit(d->label, d->nameFilters, d->filters, parent);
  /* in delegate browse button leads to program crash */
  editor->setShowBrowseButton(false);

  return editor;
}

void qPathEditDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const {
  QString value = index.model()->data(index, Qt::EditRole).toString();

  ctkPathLineEdit *pathEdit = static_cast<ctkPathLineEdit *>(editor);
  pathEdit->setCurrentPath(value);
}

void qPathEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const {
  ctkPathLineEdit *pathEdit = static_cast<ctkPathLineEdit *>(editor);
  QString value = pathEdit->currentPath();

  model->setData(index, value, Qt::EditRole);
}

void qPathEditDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
