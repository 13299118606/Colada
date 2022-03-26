// Colada includes
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ItemDelegate_p.h"
#include "qColadaH5Item.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Model.h"
#include "qREValidator.h"
#include "util.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5basecontainer.h>
#include <h5geo/h5baseobject.h>

// Qt includes
#include <QCompleter>
#include <QDir>
#include <QFileInfo>
#include <QLineEdit>
#include <QStringListModel>
#include <QDebug>

qColadaH5ItemDelegatePrivate::qColadaH5ItemDelegatePrivate(
    qColadaH5ItemDelegate &q)
    : q_ptr(&q) {}

qColadaH5ItemDelegatePrivate::~qColadaH5ItemDelegatePrivate() {}

qColadaH5ItemDelegate::qColadaH5ItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      d_ptr(new qColadaH5ItemDelegatePrivate(*this)) {}

qColadaH5ItemDelegate::~qColadaH5ItemDelegate() {}

QWidget *qColadaH5ItemDelegate::createEditor(QWidget *parent,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
  if (!index.isValid())
    return nullptr;

  QLineEdit *lineEditor = new QLineEdit(parent);
  QRegularExpression re = util::fileNameRegExp();
  qREValidator *validator =
      new qREValidator(re, util::fileNameToolTipText(), 3000, lineEditor);
  lineEditor->setValidator(validator);

  qColadaH5ProxyModel *proxyModel = qobject_cast<qColadaH5ProxyModel *>(
      const_cast<QAbstractItemModel *>(index.model()));
  if (!proxyModel){
    qCritical() << Q_FUNC_INFO << ": Unable to get proxy model";
    return lineEditor;
  }

  qColadaH5Model* model = qobject_cast<qColadaH5Model*>(proxyModel->sourceModel());
  if (!model){
    qCritical() << Q_FUNC_INFO << ": Unable to get source model";
    return lineEditor;
  }

  QModelIndex srcIndex = proxyModel->mapToSource(index);
  qColadaH5Item *item = model->itemFromIndex(srcIndex);
  if (!item || item->isRoot())
    return lineEditor;

  QStringList nameQList;
  if (item->isContainer()) {
    auto h5FileOpt = item->getH5File();
    if (h5FileOpt.has_value()){
      QFileInfo fi(QString::fromStdString(
          h5FileOpt->getFileName()));
      QDir h5Dir(fi.absolutePath());
      nameQList = h5Dir.entryList();
    }
  } else if (item->isObject() && item->getParent()) {
    qColadaH5Item *parentItem = item->getParent();
    auto h5GroupOpt = parentItem->getObjG();
    if (h5GroupOpt.has_value()){
      std::vector<std::string> nameList = h5GroupOpt->listObjectNames();
      nameQList.reserve(nameList.size());
      for (auto &name : nameList)
        nameQList.append(QString::fromStdString(name));
    }
  }
    
  QCompleter *completer = new QCompleter(lineEditor);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  completer->setCaseSensitivity(Qt::CaseSensitive);
  QStringListModel *modelList = new QStringListModel(nameQList, completer);
  completer->setModel(modelList);
  lineEditor->setCompleter(completer);

  return lineEditor;
}

void qColadaH5ItemDelegate::setEditorData(QWidget *editor,
                                          const QModelIndex &index) const {
  QString value = index.model()->data(index, Qt::EditRole).toString();
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  lineEdit->setText(value);
}

void qColadaH5ItemDelegate::setModelData(QWidget *editor,
                                         QAbstractItemModel *model,
                                         const QModelIndex &index) const {
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
  QString newName = lineEdit->text();
  QString oldName = model->data(index).toString();

  if (oldName == newName)
    return;

  qColadaH5ProxyModel *proxyModel = qobject_cast<qColadaH5ProxyModel *>(model);
  if (!proxyModel){
    qCritical() << Q_FUNC_INFO << ": Unable to get proxy model";
    return;
  }

  if (!model->setData(index, newName)) {
    qCritical() << Q_FUNC_INFO << ": Unable to rename object";
    return;
  }
}

void qColadaH5ItemDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
