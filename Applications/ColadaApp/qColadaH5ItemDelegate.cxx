// Colada includes
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ItemDelegate_p.h"
#include "qColadaH5Item.h"
#include "qColadaH5ProxyModel.h"
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
#include <QMessageBox>

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
  if (!proxyModel)
    return lineEditor;

  QModelIndex srcIndex = proxyModel->mapToSource(index);
  qColadaH5Item *item =
      static_cast<qColadaH5Item *>(srcIndex.internalPointer());
  if (item->isRoot())
    return lineEditor;

  QStringList nameQList;
  if (item->isGeoContainer()) {
    QFileInfo fi(QString::fromStdString(
        item->getGeoContainer()->getH5File().getFileName()));
    QDir h5Dir(fi.absolutePath());
    nameQList = h5Dir.entryList();
  } else if (item->isGeoObject()) {
    h5gt::Group group = item->getGeoObject()->getObjG();
    qColadaH5Item *parentItem = item->getParent();
    if (item->isGeoContainer()) {
      h5gt::File parentFile = parentItem->getGeoContainer()->getH5File();
      std::vector<std::string> nameList = parentFile.listObjectNames();
      nameQList.reserve(nameList.size());
      for (auto &name : nameList)
        nameQList.append(QString::fromStdString(name));
    } else if (item->isGeoObject()) {
      h5gt::Group parentGroup = parentItem->getGeoObject()->getObjG();
      std::vector<std::string> nameList = parentGroup.listObjectNames();
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

  if (!model->setData(index, newName)) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(tr("Invalid name!"));
    msgBox.exec();
    return;
  }
}

void qColadaH5ItemDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
  editor->setGeometry(option.rect);
}
