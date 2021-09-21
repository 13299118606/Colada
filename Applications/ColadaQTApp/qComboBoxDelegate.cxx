// Colada includes
#include "qComboBoxDelegate.h"
#include "qComboBoxDelegate_p.h"

// Qt includes
#include <QComboBox>

qComboBoxDelegatePrivate::qComboBoxDelegatePrivate(qComboBoxDelegate &q)
    : q_ptr(&q) {}

qComboBoxDelegatePrivate::~qComboBoxDelegatePrivate() {}

void qComboBoxDelegatePrivate::init(QStringList texts) {
  Q_Q(qComboBoxDelegate);
  this->texts = texts;
}

qComboBoxDelegate::qComboBoxDelegate(QStringList texts, QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(new qComboBoxDelegatePrivate(*this)){
  Q_D(qComboBoxDelegate);
  d->init(texts);
}

qComboBoxDelegate::qComboBoxDelegate(qComboBoxDelegatePrivate *pimpl, QObject *parent)
    : QStyledItemDelegate(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qComboBoxDelegate::~qComboBoxDelegate() {}

void qComboBoxDelegate::setTexts(QStringList texts) { 
  Q_D( qComboBoxDelegate);
  d->texts = texts;
}

QWidget *qComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  Q_D(const qComboBoxDelegate);
  QComboBox *editor = new QComboBox(parent);
  editor->setFrame(false);
  editor->addItems(d->texts);

  return editor;
}

void qComboBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  QString value = index.model()->data(index, Qt::EditRole).toString();

  QComboBox *combo = static_cast<QComboBox *>(editor);
  int ind = combo->findText(value, Qt::MatchFixedString);
  combo->setCurrentIndex(ind);
}

void qComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  QComboBox *combo = static_cast<QComboBox *>(editor);
  QString value = combo->currentText();

  model->setData(index, value, Qt::EditRole);
}

void qComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
