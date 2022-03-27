// Colada includes
#include "qColadaH5Model.h"
#include "qColadaH5Model_p.h"
#include "qColadaAppMainWindow.h"

// Slicer includes
#include "qSlicerApplication.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLDisplayableNode.h"

// VTK includes
#include <vtkCollection.h>

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
#include <QSet>
#include <QStack>
#include <QDebug>

// stl includes
#include <limits>
#include <filesystem>
namespace fs = std::filesystem;

qColadaH5ModelPrivate::qColadaH5ModelPrivate(qColadaH5Model &q) : q_ptr(&q) {}

qColadaH5ModelPrivate::~qColadaH5ModelPrivate() {}

void qColadaH5ModelPrivate::init(const QString &title) {
  Q_Q(qColadaH5Model);
  rootItem = new qColadaH5Item(nullptr, nullptr);
  q->initItemToBeInserted(rootItem);
  headerData = title;

  app = qSlicerApplication::application();
  if (!app){
    qCritical() << Q_FUNC_INFO << ": Unable to get application instance";
    return;
  }

  mainW = qobject_cast<qColadaAppMainWindow*>(app->mainWindow());
  if (!mainW){
    qCritical() << Q_FUNC_INFO << ": Unable to get mainwindow instance";
    return;
  }

  q->qvtkConnect(app->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
                 q, SLOT(onMRMLSceneNodeAdded(vtkObject*, vtkObject*)));
  q->qvtkConnect(app->mrmlScene(), vtkMRMLScene::NodeRemovedEvent,
                 q, SLOT(onMRMLSceneNodeRemoved(vtkObject*, vtkObject*)));

  QObject::connect(mainW, &qColadaAppMainWindow::h5FileToBeAdded,
                   q, &qColadaH5Model::onH5FileToBeAdded);
  QObject::connect(mainW, &qColadaAppMainWindow::h5FileToBeRemoved,
                   q, &qColadaH5Model::onH5FileToBeRemoved);
  QObject::connect(mainW, &qColadaAppMainWindow::h5ObjectToBeAdded,
                   q, &qColadaH5Model::onH5ObjectToBeAdded);
  QObject::connect(mainW, &qColadaAppMainWindow::h5ObjectToBeRemoved,
                   q, &qColadaH5Model::onH5ObjectToBeRemoved);
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
  return parent->getChild(index.row());
}

qColadaH5Item *qColadaH5Model::getRootItem() const { 
  Q_D(const qColadaH5Model);
  return d->rootItem; 
}

QModelIndex qColadaH5Model::index(int row, int column,
                                  const QModelIndex &parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  if (!parentItem || row < 0 || column < 0 ||
      row >= parentItem->rowCount() ||
      column >= parentItem->columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, column, parentItem);
}

QModelIndex qColadaH5Model::parent(const QModelIndex &child) const {
  if (!child.isValid())
    return QModelIndex();
  qColadaH5Item *parentItem =
      static_cast<qColadaH5Item *>(child.internalPointer());
  return indexFromItem(parentItem);
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
  if (!item)
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    return item->data();
  } else if (role == Qt::CheckStateRole) {
    return item->checkState();
  } else {
    return QVariant();
  }
}

bool qColadaH5Model::setData(
    const QModelIndex &index,
    const QVariant &value,
    int role)
{
  if (!index.isValid())
    return false;

  qColadaH5Item *item = itemFromIndex(index);
  if (!item)
    return false;

  if (role == Qt::EditRole ||
      role == Qt::DisplayRole) {
    bool result = item->setData(value.toString());
    if (result){
      // update checkstate is needed in case there are displable nodes on the Scene
      updateItemsCheckState({item});
      emit dataChanged(index, index, {Qt::EditRole, Qt::DisplayRole, Qt::CheckStateRole});
    }
    return result;
  } else if (role == Qt::CheckStateRole) {
//    item->setCheckState(static_cast<Qt::CheckState>(value.toInt()));
//    emit dataChanged(index, index, {Qt::CheckStateRole});
    emit dataChanged(index, index, {Colada::CheckBoxClickedRole});
    //setCheckStateForItemStair(item, static_cast<Qt::CheckState>(value.toInt()));
    return true;
  }

  return false;
}

QVariant qColadaH5Model::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const
{
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

bool qColadaH5Model::insertRows(int position, int rows,
                                qColadaH5Item *parentItem){
  if (!parentItem)
    return false;

  QModelIndex parent = indexFromItem(parentItem);
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
  auto itemList = parentItem->takeChildren(position, rows);
  for (auto* item : itemList)
    delete item;
  endRemoveRows();
  return true;
}

bool qColadaH5Model::removeRows(int position, int rows,
                                qColadaH5Item *parentItem){
  if (!parentItem)
    return false;

  QModelIndex parent = indexFromItem(parentItem);
  beginRemoveRows(parent, position, position + rows - 1);
  QVector<qColadaH5Item*> itemList = parentItem->takeChildren(position, rows);
  for (qColadaH5Item* item : itemList)
    delete item;
  endRemoveRows();
  return true;
}

bool qColadaH5Model::moveItem(qColadaH5Item *parentItem,
                              qColadaH5Item *item,
                              int position, bool moveH5Link)
{
  if (!item || !parentItem)
    return false;

  if (!this->canBeMoved(parentItem, item, moveH5Link))
    return false;

  qColadaH5Item* oldParentItem = item->getParent();
  if (!oldParentItem)
    return false;

  int oldPosition = item->getRow();
  if (oldPosition < 0)
    return false;

  QModelIndex oldParentIndex = indexFromItem(oldParentItem);
  if (!oldParentIndex.isValid())
    return false;

  // get path beforehand as the parent will be changed at insertChild step
  QString oldPath, itemData;
  if (moveH5Link){
    oldPath = item->getPath();
    itemData = item->data();
  }

  // remove item without deleting it
  beginRemoveRows(oldParentIndex, oldPosition, oldPosition);
  item = oldParentItem->takeChild(item);
  endRemoveRows();

  if (!item){
    return false;
  }

  // parent item must be defined after the rows were removed as there are cases
  // when parent index changes during rows remove and thus it will be invalid
  QModelIndex parentIndex = indexFromItem(parentItem);
  if (!parentIndex.isValid())
    return false;

  if (position < 0 || position > parentItem->childCount())
    position = parentItem->childCount();

  // the app crashes (when expanding) if we move item to the unfetched parent
  if (canFetchMore(parentIndex)){
    fetchMore(parentIndex);
  }

  beginInsertRows(parentIndex, position, position);
  parentItem->insertChild(item, position);
  endInsertRows();

  if (moveH5Link){
    auto parentGroupOpt = parentItem->getObjG();
    if (!parentGroupOpt->rename(oldPath.toStdString(), itemData.toStdString()))
      return false;

    parentGroupOpt->flush();
    parentItem->setChildCountInGroup(parentGroupOpt->getNumberObjects());

    auto oldParentGroupOpt = oldParentItem->getObjG();
    if (oldParentGroupOpt.has_value()){
      oldParentItem->setChildCountInGroup(oldParentGroupOpt->getNumberObjects());
    }
  }
  return true;
}

bool qColadaH5Model::canBeMoved(qColadaH5Item*& parentItem, qColadaH5Item*& item, bool moveH5Link )
{
  if (!parentItem ||
      parentItem->isRoot() ||
      !item ||
      item->isRoot() ||
      item->isContainer())
    return false;

  qColadaH5Item* oldParentItem = item->getParent();
  if (!oldParentItem)
    return false;

  // move only within the same hdf5 container
  auto oldH5FileOpt = oldParentItem->getH5File();
  auto h5FileOpt = parentItem->getH5File();
  if (!oldH5FileOpt.has_value() ||
      !h5FileOpt.has_value() ||
      oldH5FileOpt.value() != h5FileOpt.value())
    return false;

  if (moveH5Link){
    auto oldParentGroupOpt = oldParentItem->getObjG();
    auto parentGroupOpt = parentItem->getObjG();
    if (!oldParentGroupOpt.has_value() ||
        !oldParentGroupOpt->exist(item->data().toStdString()) ||
        !parentGroupOpt.has_value() ||
        parentGroupOpt->exist(item->data().toStdString()))
      return false;
  }
  return true;
}

bool qColadaH5Model::hasChildren(const QModelIndex &parent) const {
  if (parent.flags().testFlag(Qt::ItemNeverHasChildren))
    return false;

  qColadaH5Item *item = itemFromIndex(parent);
  if (!item)
    return false;

  // 'hasChildren' first of all must reflect whether the displayed item
  // has children or not and only secondly reflect the potentially existent children
  if (item->isRoot() || item->isMapped())
    return item->hasChildren();
  return item->getChildCountInGroup() > 0;
}

bool qColadaH5Model::canFetchMore(const QModelIndex &parent) const {
  qColadaH5Item *item = itemFromIndex(parent);
  return item ? !item->isMapped() : false;
}

void qColadaH5Model::refetch(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  if (!parentItem)
    return;

  parentItem->setMapped(false);
  fetchMore(parent);
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

  qColadaH5Item *item = itemFromIndex(index);
  return item ? item->getFlags() : Qt::NoItemFlags;
}

QModelIndex qColadaH5Model::indexFromItem(qColadaH5Item *item) const {
  if (item && item->getParent()) {
      int row = item->getRow();
      return createIndex(row, 0, item->getParent());
  }
  return QModelIndex();
}

QModelIndexList
qColadaH5Model::getIndexListToRoot(qColadaH5Item *item) const {
  if (!item || item->isRoot())
    return QModelIndexList();

  QList<qColadaH5Item *> itemListToRoot = item->getItemListToRoot();
  QModelIndexList indexListToRoot;
  for (auto* it : itemListToRoot) {
    if (!it)
      return QModelIndexList();

    int itemRow = it->getRow();
    if (it->isRoot()){
      indexListToRoot.push_back(QModelIndex());
    } else if (itemRow < 0){
      return QModelIndexList();
    } else {
      QModelIndex itemIndex = index(itemRow, 0, itemIndex);
      indexListToRoot.push_back(itemIndex);
    }
  }
  return indexListToRoot;
}

void qColadaH5Model::getFullChildIndexList(
    qColadaH5Item *item, QModelIndexList &fullChildIndexList)
{
  for (int i = 0; i < item->childCount(); i++) {
    QModelIndex childIndex = createIndex(i, 0, item->getChild(i));
    fullChildIndexList.push_back(childIndex);
    getFullChildIndexList(item->getChild(i), fullChildIndexList);
  }
}

void qColadaH5Model::getFullChildList(
    qColadaH5Item *item,
    QList<qColadaH5Item *> &fullChildList)
{
  for (int i = 0; i < item->childCount(); i++) {
    fullChildList.push_back(item->getChild(i));
    getFullChildList(fullChildList.last(), fullChildList);
  }
}

bool qColadaH5Model::canAddH5File(const h5gt::File& file) const {
  return true;
}

qColadaH5Item* qColadaH5Model::findItem(const QString &fileName)
{
  if (fileName.isEmpty())
    return nullptr;

  // Remeber: item may be invalid i.e. File or Group is unavailable
  // but still it must be removed
  QString name = fileName.split(QLatin1Char('/'), Qt::SkipEmptyParts).last();
  for (qColadaH5Item* child : getRootItem()->getChildren()){
    if (!child)
      continue;

    if (child->data() == name)
      return child;
  }
  return nullptr;
}

qColadaH5Item* qColadaH5Model::findItem(const h5gt::File& file)
{
  return findItem(QString::fromStdString(file.getFileName()));
}

qColadaH5Item* qColadaH5Model::findItem(const QString &fileName, const QString& objName, bool fetch)
{
  if (fileName.isEmpty() || objName.isEmpty())
    return nullptr;

  qColadaH5Item* item = nullptr;
  try {
    item = this->findItem(fileName);
    if (!item)
      return nullptr;

    QStringList objNameList = objName.split(QLatin1Char('/'), Qt::SkipEmptyParts);

    if (fetch)
      this->fetchAllChildren(this->indexFromItem(item));
    for (const auto& name: objNameList){
      if (item)
        item = item->getChildByName(name);
      else
        return nullptr;
    }
  } catch (h5gt::Exception& err) {
    qCritical() << Q_FUNC_INFO << err.what();
    return nullptr;
  }
  return item;
}

qColadaH5Item* qColadaH5Model::findItem(const h5gt::Group& objG, bool fetch)
{
  return this->findItem(QString::fromStdString(objG.getFileName()),
                        QString::fromStdString(objG.getPath()),
                        fetch);
}

qColadaH5Item* qColadaH5Model::findItem(vtkMRMLNode* node, bool fetch)
{
  auto optG = h5GroupFromNode(node);
  if (optG.has_value())
    return this->findItem(optG.value(), fetch);

  return nullptr;
}

qColadaH5Item* qColadaH5Model::insertH5File(const QString &fileName, int row)
{
  Q_D(qColadaH5Model);
  size_t childCountInGroup = 0;
  try {
    if (!fs::exists(fileName.toStdString()) || H5Fis_hdf5(fileName.toUtf8()) < 1)
      return nullptr;

    h5gt::File file(fileName.toStdString(), h5gt::File::ReadWrite);
    return insertH5File(file, row);
  } catch (h5gt::Exception& err) {
    qCritical() << Q_FUNC_INFO << err.what();
  }
  return nullptr;
}

qColadaH5Item* qColadaH5Model::insertH5File(const h5gt::File& file, int row)
{
  Q_D(qColadaH5Model);
  // if inapropriate geo container type
  if (!canAddH5File(file))
    return nullptr;

  QString fileName = QString::fromStdString(file.getFileName());
  if (d->rootItem->hasChild(fileName.split(QLatin1Char('/'), Qt::SkipEmptyParts).last()))
    return nullptr;

  if (row < 0)
    row = 0;

  if (row > d->rootItem->childCount())
    row = d->rootItem->childCount();

  qColadaH5Item *fileItem = new qColadaH5Item(fileName, d->rootItem);
  fileItem->setChildCountInGroup(file.getNumberObjects());
  initItemToBeInserted(fileItem);
  QModelIndex rootIndex = QModelIndex(); // index for ROOT

  beginInsertRows(rootIndex, row, row);
  d->rootItem->insertChild(fileItem, row);
  endInsertRows();
  return fileItem;
}

qColadaH5Item* qColadaH5Model::addH5File(const QString &fileName)
{
  return this->insertH5File(fileName, std::numeric_limits<int>::max());
}

qColadaH5Item* qColadaH5Model::addH5File(const h5gt::File& file) {
  return this->insertH5File(file, std::numeric_limits<int>::max());
}

bool qColadaH5Model::removeH5File(const QString &fullName) {
  qColadaH5Item* item = this->findItem(fullName);
  if (item)
    return this->removeRow(item->getRow());
  return false;
}

bool qColadaH5Model::removeH5File(const h5gt::File& file) {
  qColadaH5Item* item = this->findItem(file);
  if (item)
    return this->removeRow(item->getRow());
  return false;
}

qColadaH5Item* qColadaH5Model::insertH5Object(
    const QString& fileName, const QString& objName, int row)
{
  Q_D(qColadaH5Model);
  qColadaH5Item* parentItem = findItem(fileName);
  if (!parentItem)
    parentItem = insertH5File(fileName, std::numeric_limits<int>::max());

  if (!parentItem)
    return nullptr;

  qColadaH5Item *item = nullptr;
  QStringList names = objName.split(QLatin1Char('/'), Qt::SkipEmptyParts);
  for (const auto& name : names){
    qColadaH5Item* child = parentItem->getChildByName(name);
    if (child){
      parentItem = child;
    } else {
      auto parentG = parentItem->getObjG();
      if (!parentG.has_value() ||
          !parentG->hasObject(name.toStdString(), h5gt::ObjectType::Group))
        return nullptr;

      if (parentItem->hasChild(name))
        continue;

      // update in case the data has been changed
      parentItem->setChildCountInGroup(parentG->getNumberObjects());

      auto objG = parentG->getGroup(name.toStdString());
      item = new qColadaH5Item(name, parentItem);
      item->setObjectType(static_cast<unsigned>(h5geo::getGeoObjectType(objG)));
      item->setLinkType(parentG->getLinkType(name.toStdString()));
      item->setChildCountInGroup(objG.getNumberObjects());
      initItemToBeInserted(item);
      if (row < 0)
        row = 0;

      if (row > parentItem->childCount())
        row = parentItem->childCount();

      QModelIndex parentIndex = this->indexFromItem(parentItem);
      beginInsertRows(parentIndex, row, row);
      parentItem->insertChild(item, row);
      endInsertRows();
      parentItem = item;
    }
  }
  return item;
}

qColadaH5Item* qColadaH5Model::insertH5Object(const h5gt::Group& objG, int row)
{
  return insertH5Object(QString::fromStdString(objG.getFileName()),
                        QString::fromStdString(objG.getPath()), row);
}

qColadaH5Item* qColadaH5Model::addH5Object(
    const QString& fileName, const QString& objName)
{
  return insertH5Object(fileName, objName, std::numeric_limits<int>::max());
}

qColadaH5Item* qColadaH5Model::addH5Object(const h5gt::Group& objG)
{
  return insertH5Object(objG, std::numeric_limits<int>::max());
}

void qColadaH5Model::initItemToBeInserted(qColadaH5Item* item) const
{
  if (!item)
    return;

  Qt::ItemFlags flags =
      Qt::ItemIsSelectable |
      Qt::ItemIsEnabled;

  unsigned objType = item->getObjectType();
  if (objType > 0){
    flags |=
        Qt::ItemIsUserCheckable |
        Qt::ItemIsEditable |
        Qt::ItemIsDragEnabled |
        Qt::ItemIsDropEnabled;
  }
  item->setFlags(flags);
}

bool qColadaH5Model::removeH5Object(
    const QString& fileName, const QString& objName, bool unlink)
{
  if (fileName.isEmpty() || objName.isEmpty())
    return false;

  try {
    if (!fs::exists(fileName.toStdString()) || H5Fis_hdf5(fileName.toUtf8()) < 1)
      return false;

    h5gt::File file(fileName.toStdString(), h5gt::File::ReadWrite);
    if (!file.hasObject(objName.toStdString(), h5gt::ObjectType::Group))
      return false;

    h5gt::Group objG = file.getGroup(objName.toStdString());
    return removeH5Object(objG, unlink);
  } catch (h5gt::Exception& err) {
    qCritical() << Q_FUNC_INFO << err.what();
    return false;
  }
}

bool qColadaH5Model::qColadaH5Model::removeH5Object(
    const h5gt::Group& objG, bool unlink)
{
  qColadaH5Item* item = this->findItem(objG);
  return this->removeItem(item, unlink);
}

bool qColadaH5Model::removeItem(qColadaH5Item* item, bool unlink)
{
  if (!item)
    return false;

  int row = item->getRow();
  if (row < 0)
    return false;

  QString itemData = item->data(); // item will be destroyed after removing row
  qColadaH5Item* parentItem = item->getParent();
  if (!parentItem)
    return false;

  QModelIndex parentIndex = indexFromItem(parentItem);
  if (!removeRow(row, parentIndex))
    return false;

  // item is already destroyed here
  if (unlink){
    auto parentG_opt = parentItem->getObjG();
    if (parentG_opt.has_value()){
      parentG_opt->unlink(itemData.toStdString());
      parentG_opt->flush();
      parentItem->setChildCountInGroup(parentG_opt->getNumberObjects());
    }
  }
  return true;
}

void qColadaH5Model::releaseCheckState(qColadaH5Item *topLevelItem) {
  topLevelItem->setCheckState(Qt::Unchecked);
  for (int i = 0; i < topLevelItem->childCount(); i++) {
    releaseCheckState(topLevelItem->getChild(i));
  }
}

void qColadaH5Model::updateCheckState(qColadaH5Item *topLevelItem) {
  topLevelItem->setCheckState(topLevelItem->checkState());
  for (int i = 0; i < topLevelItem->childCount(); i++) {
    updateCheckState(topLevelItem->getChild(i));
  }
}

void qColadaH5Model::sendItemDataChanged(
    qColadaH5Item* item, const QVector<int> &roles)
{
  if (!item)
    return;

  int row = item->getRow();
  if (row < 0) // if this is a root item
    emit dataChanged(QModelIndex(), QModelIndex(), roles);
  else {
    QModelIndex index = createIndex(item->getRow(), 0, item);
    emit dataChanged(index, index, roles);
  }
}

void qColadaH5Model::sendAllChildDataChanged(
    qColadaH5Item *topLevelItem, const QVector<int> &roles)
{
  if (!topLevelItem)
    return;

  QModelIndexList fullChildIndexList;
  getFullChildIndexList(topLevelItem, fullChildIndexList);
  for (const QModelIndex& index : fullChildIndexList){
    emit dataChanged(index, index, roles);
  }
}

void qColadaH5Model::sendAllItemsToRootDataChanged(
    qColadaH5Item *lowLevelItem,
    const QVector<int> &roles)
{
  if (!lowLevelItem)
    return;

  QModelIndexList indexListToRoot = getIndexListToRoot(lowLevelItem);
  for (int i = 0; i < indexListToRoot.count(); i++) {
    emit dataChanged(indexListToRoot[i], indexListToRoot[i], roles);
  }
}

void qColadaH5Model::setCheckStateForItemStair(
    qColadaH5Item *item,
    Qt::CheckState checkState)
{
  if (!item || item->isRoot())
    return;

  QList<qColadaH5Item *> itemListToRoot = item->getItemListToRoot();
  QModelIndexList indexListToRoot = getIndexListToRoot(item);
  if (itemListToRoot.count() != indexListToRoot.count())
    return;

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
  QModelIndexList fullChildIndexList;
  getFullChildIndexList(item, fullChildIndexList);
  for (int i = 0; i < fullChildList.count(); i++) {
    if (fullChildList[i]->checkState() != checkState) {
      fullChildList[i]->setCheckState(checkState);
      emit dataChanged(fullChildIndexList[i], fullChildIndexList[i],
                       {Qt::CheckStateRole});
    }
  }
}

void qColadaH5Model::updateItemsCheckState(const QVector<qColadaH5Item*>& items)
{
  Q_D(qColadaH5Model);
  std::vector<vtkMRMLNode*> nodes;
  d->app->mrmlScene()->GetNodesByClass("vtkMRMLDisplayableNode", nodes);
  for (qColadaH5Item* item : items){
    if (!item)
      continue;

    auto opt = item->getObjG();
    if (!opt.has_value())
      continue;

    bool nodeFound = false;
    for (vtkMRMLNode* node : nodes){
      std::optional<h5gt::Group> nodeGroupOpt = h5GroupFromNode(node);
      if (!nodeGroupOpt.has_value())
        continue;

      if (opt.value() == nodeGroupOpt.value()){
        item->setCheckState(Qt::Checked);
        nodeFound = true;
        break;
      }
    }

    // we need to uncheck item if there is no appropriate node found
    if (!nodeFound && item->checkState()){
      item->setCheckState(Qt::Unchecked);
    }
  }
}

static inline QString qColadaH5ModelDataListMimeType()
{
  return QStringLiteral("application/x-qcoladah5modeldatalist");
}

QStringList qColadaH5Model::mimeTypes() const
{
  return QAbstractItemModel::mimeTypes() << qColadaH5ModelDataListMimeType();
}

QMimeData *qColadaH5Model::mimeData(const QModelIndexList &indexes) const
{
  QMimeData *data = QAbstractItemModel::mimeData(indexes);
  if(!data)
    return 0;
  const QString format = qColadaH5ModelDataListMimeType();
  if (!mimeTypes().contains(format))
    return data;
  QByteArray encoded;
  QDataStream stream(&encoded, QIODevice::WriteOnly);
  QSet<qColadaH5Item*> itemsSet;
  QStack<qColadaH5Item*> stack;
  itemsSet.reserve(indexes.count());
  stack.reserve(indexes.count());
  for (int i = 0; i < indexes.count(); ++i) {
    if (qColadaH5Item *item = itemFromIndex(indexes.at(i))) {
      itemsSet << item;
      stack.push(item);
    } else {
      qWarning() << Q_FUNC_INFO << "No item associated with invalid index";
      return 0;
    }
  }
  //remove duplicates childrens
  {
    QSet<qColadaH5Item *> seen;
    while (!stack.isEmpty()) {
      qColadaH5Item *itm = stack.pop();
      if (seen.contains(itm))
        continue;
      seen.insert(itm);
      const QVector<qColadaH5Item*> &childList = itm->getChildren();
      for (int i = 0; i < childList.count(); ++i) {
        qColadaH5Item *chi = childList.at(i);
        if (chi) {
          itemsSet.erase(itemsSet.constFind(chi));
          stack.push(chi);
        }
      }
    }
  }
  stack.reserve(itemsSet.count());
  for (qColadaH5Item *item : qAsConst(itemsSet))
    stack.push(item);
  //stream everything recursively
  while (!stack.isEmpty()) {
    qColadaH5Item *item = stack.pop();
    stream << *item;
    stack += item->getChildren();
  }
  data->setData(format, encoded);
  return data;
}

std::optional<h5gt::Group> qColadaH5Model::h5GroupFromNode(vtkMRMLNode* node)
{
  if (!node ||
      !node->GetAttribute("GeoContainer") ||
      !node->GetAttribute("GeoObject"))
    return std::nullopt;

  auto fileName = node->GetAttribute("GeoContainer");
  if (!fileName)
    return std::nullopt;

  auto objName = node->GetAttribute("GeoObject");
  if (!objName)
    return std::nullopt;

  try {
    if (!fs::exists(std::string(fileName)) || H5Fis_hdf5(fileName) < 1)
      return std::nullopt;

    h5gt::File h5File(fileName, h5gt::File::ReadWrite);
    if (h5File.hasObject(objName, h5gt::ObjectType::Group))
      return h5File.getGroup(objName);
  } catch (h5gt::Exception& err) {
    qCritical() << Q_FUNC_INFO << err.what();
  }
  return std::nullopt;
}

void qColadaH5Model::onMRMLSceneNodeAdded(
    vtkObject*, vtkObject* node)
{
  Q_D(qColadaH5Model);
  vtkMRMLDisplayableNode* dispNode = vtkMRMLDisplayableNode::SafeDownCast(node);
  if (!dispNode)
    return;

  qColadaH5Item* item = this->findItem(dispNode);
  if (!item)
    return;

  item->setCheckState(Qt::Checked);
  QModelIndex index = indexFromItem(item);
  emit dataChanged(index, index, {Qt::CheckStateRole});
}

void qColadaH5Model::onMRMLSceneNodeRemoved(
    vtkObject*, vtkObject* node)
{
  Q_D(qColadaH5Model);
  vtkMRMLDisplayableNode* dispNodeRem = vtkMRMLDisplayableNode::SafeDownCast(node);
  if (!dispNodeRem)
    return;

  qColadaH5Item* itemRem = this->findItem(dispNodeRem);
  if (!itemRem)
    return;

  std::vector<vtkMRMLNode*> nodes;
  d->app->mrmlScene()->GetNodesByClass("vtkMRMLDisplayableNode", nodes);
  for (vtkMRMLNode* object : nodes){
    vtkMRMLDisplayableNode* dispNodeExist = vtkMRMLDisplayableNode::SafeDownCast(object);
    if (!dispNodeExist)
      continue;

    // if nodes with the same geo object exist on the scene then don't uncheck it
    qColadaH5Item* itemExist = this->findItem(dispNodeExist);
    if (!itemExist)
      continue;

    if (*itemRem == *itemExist)
      return;
  }

  itemRem->setCheckState(Qt::Unchecked);
  QModelIndex index = indexFromItem(itemRem);
  emit dataChanged(index, index, {Qt::CheckStateRole});
}

void qColadaH5Model::onH5FileToBeRemoved(
    const QString& fileName)
{
  this->removeH5File(fileName);
}

void qColadaH5Model::onH5FileToBeAdded(
    const QString& fileName)
{
  this->addH5File(fileName);
}

void qColadaH5Model::onH5ObjectToBeRemoved(
    const QString& fileName, const QString& objName)
{
  this->removeH5Object(fileName, objName);
}

void qColadaH5Model::onH5ObjectToBeAdded(
    const QString& fileName, const QString& objName)
{
  this->addH5Object(fileName, objName);
}

bool qColadaH5Model::canDropMimeData(
    const QMimeData *data,
    Qt::DropAction action,
    int row,
    int column,
    const QModelIndex &parent) const
{
  if (action == Qt::IgnoreAction)
    return true;

  qColadaH5Item* parentItem = nullptr;
  qColadaH5Item* item = nullptr;
  std::string newObjectName;
  return const_cast<qColadaH5Model*>(this)->canDropMimeDataAndPrepare(
        data, row, column, parent,
        parentItem, item);
}

bool qColadaH5Model::dropMimeData(
    const QMimeData *data,
    Qt::DropAction action,
    int row,
    int column,
    const QModelIndex &parent)
{
  if (action == Qt::IgnoreAction)
    return true;

  qColadaH5Item* parentItem = nullptr;
  qColadaH5Item* item = nullptr;
  if (!canDropMimeDataAndPrepare(
      data, row, column, parent,
      parentItem, item))
    return false;

  return this->moveItem(parentItem, item, row, true);
}

bool qColadaH5Model::canDropMimeDataAndPrepare(
    const QMimeData *data,
    int row,
    int column,
    const QModelIndex &parent,
    qColadaH5Item*& parentItem,
    qColadaH5Item*& item)
{
  QStringList typeList = this->mimeTypes();
  if (typeList.isEmpty())
    return false;

  if (!data || !data->hasFormat(typeList.last()))
    return false;

  parentItem = this->itemFromIndex(parent);
  if (!parentItem)
    return false;

  QString fileName, objectName;
  QByteArray encodedData = data->data(typeList.last());
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  stream >> fileName >> objectName;
  if (objectName == "/")
    return false;

  item = findItem(fileName, objectName);
  if (!item || !item->isObject() || parentItem->isSame(item))
    return false;

  qColadaH5Item* oldParentItem = item->getParent();
  if (!oldParentItem)
    return false;

  // prevent from dropping parent to child
  // adding '/' at the end is important for example
  // if parent path is '/gg' and item path is '/g'
  // then 'g' is not a child of 'gg' but I won't be
  // able to drop an item there without '/' at end
  QString parentPath = parentItem->getPath() + "/";
  QString itemPath = item->getPath() + "/";
  if (parentPath.contains(itemPath, Qt::CaseInsensitive))
    return false;

  return this->canBeMoved(parentItem, item, true);
}
