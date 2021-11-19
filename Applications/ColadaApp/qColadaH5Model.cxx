// Colada includes
#include "qColadaH5Model.h"
#include "qColadaH5Model_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>
#include <h5gt/H5DataSet.hpp>

// h5geo includes
#include <h5geo/h5basecontainer.h>
#include <h5geo/h5baseobject.h>

// Qt includes
#include <QMimeData>
#include <QDataStream>

qColadaH5ModelPrivate::qColadaH5ModelPrivate(qColadaH5Model &q) : q_ptr(&q) {}

qColadaH5ModelPrivate::~qColadaH5ModelPrivate() {}

void qColadaH5ModelPrivate::init(const QString &title) {
  rootItem = new qColadaH5Item(nullptr, nullptr);
  headerData = title;
}

qColadaH5Model::qColadaH5Model(QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new qColadaH5ModelPrivate(*this)) {
  Q_D(qColadaH5Model);
  d->init("");
}

qColadaH5Model::qColadaH5Model(const QString &title, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(new qColadaH5ModelPrivate(*this)) {
  Q_D(qColadaH5Model);
  d->init(title);
}

qColadaH5Model::qColadaH5Model(
  qColadaH5ModelPrivate *pimpl, QObject *parent)
    : QAbstractItemModel(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qColadaH5Model::~qColadaH5Model() { 
  Q_D(qColadaH5Model);
  delete d->rootItem; 
}

void qColadaH5Model::setTitle(const QString &title) { 
  Q_D(qColadaH5Model);
  d->headerData = title;
}

QString qColadaH5Model::getTitle() {
  Q_D(qColadaH5Model);
  return d->headerData;
}

qColadaH5Item *qColadaH5Model::itemFromIndex(const QModelIndex &index) const {
  Q_D(const qColadaH5Model);
  if (!index.isValid())
    return d->rootItem;
  if (index.model() != this)
    return nullptr;
  qColadaH5Item *parent = static_cast<qColadaH5Item *>(index.internalPointer());
  if (parent == nullptr)
    return nullptr;
  return parent->getChildItem(index.row());
}

qColadaH5Item *qColadaH5Model::getRootItem() const { 
  Q_D(const qColadaH5Model);
  return d->rootItem; 
}

QModelIndex qColadaH5Model::index(int row, int column,
                                  const QModelIndex &parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  if ((parentItem == 0) || (row < 0) || (column < 0) ||
      (row >= parentItem->rowCount()) ||
      (column >= parentItem->columnCount())) {
    return QModelIndex();
  }
  return createIndex(row, column, parentItem);
}

QModelIndex qColadaH5Model::parent(const QModelIndex &child) const {
  if (!child.isValid())
    return QModelIndex();
  qColadaH5Item *parentItem =
      static_cast<qColadaH5Item *>(child.internalPointer());
  return getIndex(parentItem);
}

int qColadaH5Model::rowCount(const QModelIndex &parent) const {
  const qColadaH5Item *parentItem = itemFromIndex(parent);

  return parentItem ? parentItem->rowCount() : 0;
}

int qColadaH5Model::columnCount(const QModelIndex &parent) const {
  Q_D(const qColadaH5Model);
  return d->rootItem->columnCount();
}

QVariant qColadaH5Model::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  qColadaH5Item *item = itemFromIndex(index);
  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return item->data();
  } else if (role == Qt::CheckStateRole) {
    return item->checkState();
  } else {
    return QVariant();
  }
}

bool qColadaH5Model::setData(const QModelIndex &index, const QVariant &value,
                             int role) {
  if (!index.isValid())
    return false;

  qColadaH5Item *item = itemFromIndex(index);
  if (role == Qt::EditRole) {
    bool result = item->setData(value.toString());
    emit dataChanged(index, index, {Qt::EditRole});
    return result;
  } else if (role == Qt::DisplayRole) {
    bool result = item->setData(value.toString());
    emit dataChanged(index, index, {Qt::DisplayRole});
    return result;
  } else if (role == Qt::CheckStateRole) {
    item->setCheckState(static_cast<Qt::CheckState>(value.toInt()));
    emit dataChanged(index, index, {Qt::CheckStateRole});
    //setCheckStateForItemStair(item, static_cast<Qt::CheckState>(value.toInt()));
    return true;
  }

  return false;
}

QVariant qColadaH5Model::headerData(int section, Qt::Orientation orientation,
                                    int role) const {
  Q_D(const qColadaH5Model);
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole &&
      section == 0) {
    return d->headerData;
  }
  return QVariant();
}

bool qColadaH5Model::insertRows(int position, int rows,
                                const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  if (!parentItem)
    return false;
  
  beginInsertRows(parent, position, position + rows - 1);
  endInsertRows();

  return true;
}

bool qColadaH5Model::removeRows(int position, int rows,
                                const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  if (!parentItem)
    return false;

  beginRemoveRows(parent, position, position + rows - 1);
  const bool success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

bool qColadaH5Model::hasChildren(const QModelIndex &parent) const {
  qColadaH5Item *item = itemFromIndex(parent);
  return item ? item->hasChildren() : false;
}

bool qColadaH5Model::canFetchMore(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return false;
  }

  qColadaH5Item *item = itemFromIndex(parent);
  return !item->isMapped();
}

void qColadaH5Model::fetchAllChildren(const QModelIndex &parent) {
  if (!canFetchMore(parent))
    return;

  fetchMore(parent);
  for (int i = 0; i < rowCount(parent); i++) {
    fetchAllChildren(index(i, 0, parent));
  }
}

Qt::ItemFlags qColadaH5Model::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEditable | Qt::ItemIsUserCheckable |
         QAbstractItemModel::flags(index);
}

QModelIndex qColadaH5Model::getIndex(qColadaH5Item *item) const {
  if (item->isRoot())
    return QModelIndex();

  QList<qColadaH5Item *> itemListToRoot = getItemListToRoot(item);
  QList<QModelIndex> indexListToRoot = getIndexListToRoot(item);

  return indexListToRoot.last();
}

QList<qColadaH5Item *>
qColadaH5Model::getItemListToRoot(qColadaH5Item *item) const {
  QList<qColadaH5Item *> itemListToRoot;
  if (item->isRoot())
    return itemListToRoot;

  while (!item->isRoot()) {
    itemListToRoot.push_back(item);
    item = item->getParentItem();
  }
  itemListToRoot.push_back(item);
  /* we want ROOT to be 0 element */
  std::reverse(itemListToRoot.begin(), itemListToRoot.end());
  return itemListToRoot;
}

QList<QModelIndex>
qColadaH5Model::getIndexListToRoot(qColadaH5Item *item) const {
  QList<qColadaH5Item *> itemListToRoot = getItemListToRoot(item);
  QList<QModelIndex> indexListToRoot;
  QModelIndex itemIndex = QModelIndex(); // index for ROOT
  indexListToRoot.push_back(itemIndex);
  for (int i = 1; i < itemListToRoot.count(); i++) {
    itemIndex = index(itemListToRoot[i]->getRow(), 0, itemIndex);
    indexListToRoot.push_back(itemIndex);
  }
  return indexListToRoot;
}

QList<QModelIndex> qColadaH5Model::getIndexListToRoot(
    qColadaH5Item *item, const QList<qColadaH5Item *> &itemListToRoot) {
  if (itemListToRoot.isEmpty())
    return QList<QModelIndex>();

  QList<QModelIndex> indexListToRoot;
  QModelIndex itemIndex = QModelIndex(); // index for ROOT
  indexListToRoot.push_back(itemIndex);
  for (int i = 1; i < itemListToRoot.count(); i++) {
    itemIndex = index(itemListToRoot[i]->getRow(), 0, itemIndex);
    indexListToRoot.push_back(itemIndex);
  }
  return indexListToRoot;
}

void qColadaH5Model::getFullChildIndexList(
    qColadaH5Item *item, QList<QModelIndex> &fullChildIndexList) {
  for (int i = 0; i < item->childCount(); i++) {
    QModelIndex childIndex = createIndex(i, 0, item->getChildItem(i));
    fullChildIndexList.push_back(childIndex);
    getFullChildIndexList(item->getChildItem(i), fullChildIndexList);
  }
}

void qColadaH5Model::getFullChildList(qColadaH5Item *item,
                                      QList<qColadaH5Item *> &fullChildList) {
  for (int i = 0; i < item->childCount(); i++) {
    fullChildList.push_back(item->getChildItem(i));
    getFullChildList(fullChildList.last(), fullChildList);
  }
}

bool qColadaH5Model::canAddH5File(h5gt::File file) const { return true; }

bool qColadaH5Model::addH5File(const QString &fullName) {
  
  if (H5Fis_hdf5(fullName.toUtf8()) <= 0)
    return false;

  h5gt::File file(fullName.toStdString(), h5gt::File::ReadWrite);
  return addH5File(file);
}

bool qColadaH5Model::addH5File(h5gt::File file) { 
  Q_D(qColadaH5Model);
  if (!canAddH5File(file))
    return false;

  H5BaseContainer *baseCnt = h5geo::openBaseContainer(file);
  qColadaH5Item *fileItem = new qColadaH5Item(baseCnt, d->rootItem);
  QModelIndex rootIndex = QModelIndex(); // index for ROOT

  beginInsertRows(rootIndex, d->rootItem->childCount(),
                  d->rootItem->childCount());
  d->rootItem->insertChild(fileItem, d->rootItem->childCount());
  endInsertRows();
  return true;
}

void qColadaH5Model::releaseCheckState(qColadaH5Item *topLevelItem) {
  topLevelItem->setCheckState(Qt::Unchecked);
  for (int i = 0; i < topLevelItem->childCount(); i++) {
    releaseCheckState(topLevelItem->getChildItem(i));
  }
}

void qColadaH5Model::updateCheckState(qColadaH5Item *topLevelItem) {
  topLevelItem->setCheckState(topLevelItem->checkState());
  for (int i = 0; i < topLevelItem->childCount(); i++) {
    updateCheckState(topLevelItem->getChildItem(i));
  }
}

void qColadaH5Model::sendItemDataChanged(
    qColadaH5Item* item, const QVector<int> &roles)
{
  int row = item->getRow();
  if (row < 0) // if this is a root item
    emit dataChanged(QModelIndex(), QModelIndex(), roles);
  else {
    QModelIndex index = createIndex(item->getRow(), 0, item);
    emit dataChanged(index, index, roles);
  }
}

void qColadaH5Model::sendAllChildDataChanged(
    qColadaH5Item *topLevelItem, const QVector<int> &roles) {
  QList<QModelIndex> fullChildIndexList;
  getFullChildIndexList(topLevelItem, fullChildIndexList);
  for (const QModelIndex& index : fullChildIndexList){
    emit dataChanged(index, index, roles);
  }
}

void qColadaH5Model::sendAllItemsToRootDataChanged(qColadaH5Item *lowLevelItem,
                                                   const QVector<int> &roles) {
  QList<QModelIndex> indexListToRoot = getIndexListToRoot(lowLevelItem);
  for (int i = 0; i < indexListToRoot.count(); i++) {
    emit dataChanged(indexListToRoot[i], indexListToRoot[i], roles);
  }
}

void qColadaH5Model::setCheckStateForItemStair(qColadaH5Item *item,
                                               Qt::CheckState checkState) {

  QList<qColadaH5Item *> itemListToRoot = getItemListToRoot(item);
  QList<QModelIndex> indexListToRoot = getIndexListToRoot(item, itemListToRoot);

  fetchAllChildren(indexListToRoot.last());

  Qt::CheckState oldCheckState = item->checkState();
  checkState =
      oldCheckState == Qt::PartiallyChecked ? Qt::Unchecked : checkState;
  item->setCheckState(checkState);
  emit dataChanged(indexListToRoot.last(), indexListToRoot.last(),
                   {Qt::CheckStateRole});

  Qt::CheckState parentCheckState =
      checkState == Qt::Unchecked ? Qt::Unchecked : Qt::PartiallyChecked;

  if (parentCheckState == Qt::PartiallyChecked) {
    for (int i = 1; i < itemListToRoot.count() - 1;
         i++) { // exclude item and root
      if (itemListToRoot[i]->checkState() != Qt::PartiallyChecked) {
        itemListToRoot[i]->setCheckState(parentCheckState);
        emit dataChanged(indexListToRoot[i], indexListToRoot[i],
                         {Qt::CheckStateRole});
      }
    }
  } else { // checkState == Qt::Unchecked
    for (int i = itemListToRoot.count() - 2; i > 0;
         i--) { // exclude item and root
      if (!itemListToRoot[i]->isEvenOneChildChecked()) {
        itemListToRoot[i]->setCheckState(parentCheckState);
        emit dataChanged(indexListToRoot[i], indexListToRoot[i],
                         {Qt::CheckStateRole});
      }
    }
  }

  QList<qColadaH5Item *> fullChildList;
  getFullChildList(item, fullChildList);
  QList<QModelIndex> fullChildIndexList;
  getFullChildIndexList(item, fullChildIndexList);
  for (int i = 0; i < fullChildList.count(); i++) {
    if (fullChildList[i]->checkState() != checkState) {
      fullChildList[i]->setCheckState(checkState);
      emit dataChanged(fullChildIndexList[i], fullChildIndexList[i],
                       {Qt::CheckStateRole});
    }
  }
}
