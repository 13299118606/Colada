// Colada includes
#include "qColadaH5Item.h"
#include "qColadaH5Item_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>
#include <h5gt/H5DataSet.hpp>

// Qt includes
#include <QStringList>

qColadaH5ItemPrivate::qColadaH5ItemPrivate(qColadaH5Item &q) : q_ptr(&q) {}

qColadaH5ItemPrivate::~qColadaH5ItemPrivate() {}

qColadaH5Item::qColadaH5Item(H5Base *itemData, qColadaH5Item *parent)
    : d_ptr(new qColadaH5ItemPrivate(*this)) {
  Q_D(qColadaH5Item);

  d->parentItem = parent;
  d->itemData = H5Base_ptr(itemData);
}

qColadaH5Item::~qColadaH5Item() {
  Q_D(qColadaH5Item);
  qDeleteAll(d->childItems);
}

int qColadaH5Item::childCount() const {
  Q_D(const qColadaH5Item);
  return d->childItems.count();
}

int qColadaH5Item::childNumber() const {
  Q_D(const qColadaH5Item);
  if (d->parentItem)
    return d->parentItem->getChildren().indexOf(
        const_cast<qColadaH5Item *>(this));
  return 0;
}

int qColadaH5Item::rowCount() const { 
  return childCount();
}

int qColadaH5Item::columnCount() const { return 1; }

void qColadaH5Item::appendChild(qColadaH5Item *item) {
  Q_D(qColadaH5Item);
  d->childItems.append(item);
}

void qColadaH5Item::appendChildren(QVector<qColadaH5Item *> itemList) {
  Q_D(qColadaH5Item);
  d->childItems.append(itemList);
}

bool qColadaH5Item::insertChild(qColadaH5Item *item, int position) {
  Q_D(qColadaH5Item);
  if (position < 0 || position > d->childItems.size())
    return false;

  d->childItems.insert(position, item);
  return true;
}

bool qColadaH5Item::removeChild(int position) {
  Q_D(qColadaH5Item);
  if (position < 0 || position >= d->childItems.size())
    return false;

  delete d->childItems.takeAt(position);

  return true;
}

bool qColadaH5Item::removeChildren(int position, int count) {
  Q_D(qColadaH5Item);
  if (position < 0 || position + count > d->childItems.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete d->childItems.takeAt(position);

  return true;
}

bool qColadaH5Item::hasChild(const QString &name) const {
  Q_D(const qColadaH5Item);
  for (auto &item : d->childItems)
    if (item->data().compare(name, Qt::CaseInsensitive) == 0)
      return true;

  return false;
}

bool qColadaH5Item::hasChildren() const { 
  Q_D(const qColadaH5Item);
  if (isGeoContainer()) {
    H5BaseContainer *obj = getGeoContainer();
    return obj->getH5File().getNumberObjects() > 0;
  } else if (isGeoObject()) {
    H5BaseObject *obj = getGeoObject();
    return obj->getObjG().getNumberObjects() > 0;
  }

  return childCount() > 0;
}

QString qColadaH5Item::data() const {
  Q_D(const qColadaH5Item);
  if (isGeoContainer()) {
    H5BaseContainer *obj = getGeoContainer();
    return QString::fromStdString(obj->getH5File().getFileName())
        .split("/")
        .takeLast();
  } else if (isGeoObject()) {
    H5BaseObject *obj = getGeoObject();
    return QString::fromStdString(obj->getObjG().getPath())
        .split("/")
        .takeLast();
   }
  return "";
}

bool qColadaH5Item::setData(const QString &newItemData) {
  Q_D(qColadaH5Item);
  qColadaH5Item* parent = getParent();
  if (!parent)
    return false;

  QString oldItemData = this->data();
  if (parent->isGeoContainer() && this->isGeoObject()){
    h5gt::File parentFile = parent->getGeoContainer()->getH5File();
    if (!parentFile.exist(oldItemData.toStdString()) ||
        parentFile.exist(newItemData.toStdString()))
      return false;

    parentFile.rename(oldItemData.toStdString(), newItemData.toStdString());
    parentFile.flush();
    return true;
  } else if (parent->isGeoObject() && this->isGeoObject()){
    h5gt::Group parentGroup = parent->getGeoObject()->getObjG();
    if (!parentGroup.exist(oldItemData.toStdString()) ||
        parentGroup.exist(newItemData.toStdString()))
      return false;

    parentGroup.rename(oldItemData.toStdString(), newItemData.toStdString());
    parentGroup.flush();
    return true;
  }
  return false;
}

qint64 qColadaH5Item::findRow(qColadaH5Item *item)
{
  Q_D(qColadaH5Item);
  QVector<qColadaH5Item *>::Iterator it =
      std::find(d->childItems.begin(), d->childItems.end(), item);
  if (it == d->childItems.end())
    return -1;

  return std::distance(d->childItems.begin(), it); // should be the same as: it - itemList.begin()
}

qColadaH5Item *qColadaH5Item::getChildByName(
    const QString &name) const
{
  Q_D(const qColadaH5Item);
  for (auto &item : d->childItems)
    if (item->data().compare(name, Qt::CaseInsensitive) == 0)
      return item;

  return nullptr;
}

qColadaH5Item *qColadaH5Item::getChild(int number) const {
  Q_D(const qColadaH5Item);
  if (number < 0 || number >= d->childItems.size())
    return nullptr;
  return d->childItems.at(number);
}

QVector<qColadaH5Item *> qColadaH5Item::getChildren() const {
  Q_D(const qColadaH5Item);
  return d->childItems;
}

qColadaH5Item *qColadaH5Item::getParent() const {
  Q_D(const qColadaH5Item);
  return d->parentItem;
}

int qColadaH5Item::getRow() const {
  if (this->getParent() == nullptr)
    return -1;

  qColadaH5Item *parent = this->getParent();
  for (int i = 0; i < parent->childCount(); i++)
    if (parent->getChild(i) == this)
      return i;

  return -1;
}

int qColadaH5Item::getColumn() const {
  return 0;
}

QList<qColadaH5Item *> qColadaH5Item::getItemListToRoot() {
  QList<qColadaH5Item *> itemListToRoot;
  if (isRoot())
    return itemListToRoot;

  qColadaH5Item *item = this;
  while (!item->isRoot()) {
    itemListToRoot.push_back(item);
    item = item->getParent();
  }
  itemListToRoot.push_back(item);
  /* we want ROOT to be 0 element */
  std::reverse(itemListToRoot.begin(), itemListToRoot.end());
  return itemListToRoot;
}

H5Base* qColadaH5Item::getItemData() const { 
  Q_D(const qColadaH5Item);
  return d->itemData.get();
}

H5BaseContainer* qColadaH5Item::getGeoContainer() const {
  Q_D(const qColadaH5Item);
  return dynamic_cast<H5BaseContainer *>(d->itemData.get());
}

H5BaseObject* qColadaH5Item::getGeoObject() const {
  Q_D(const qColadaH5Item);
  return dynamic_cast<H5BaseObject *>(d->itemData.get());
}

bool qColadaH5Item::setChildren(QVector<qColadaH5Item *> childItems) {
  Q_D(qColadaH5Item);
  d->childItems = childItems;
  return true;
}

bool qColadaH5Item::setParent(qColadaH5Item *parentItem) {
  Q_D(qColadaH5Item);
  d->parentItem = parentItem;
  return true;
}

void qColadaH5Item::setCheckState(Qt::CheckState checkState) {
  Q_D(qColadaH5Item);
  d->checkState = checkState;
}

void qColadaH5Item::setMapped(bool val) {
  Q_D(qColadaH5Item);
  d->mapped = val;
}

bool qColadaH5Item::isRoot() const {
  Q_D(const qColadaH5Item);
  if (d->itemData == nullptr && d->parentItem == nullptr)
    return true;

  return false;
}

bool qColadaH5Item::isGeoContainer() const {
  Q_D(const qColadaH5Item);
  if (dynamic_cast<H5BaseContainer *>(d->itemData.get()))
    return true;

  return false;
}

bool qColadaH5Item::isGeoObject() const {
  Q_D(const qColadaH5Item);
  if (dynamic_cast<H5BaseObject *>(d->itemData.get()))
    return true;

  return false;
}

Qt::CheckState qColadaH5Item::checkState() const {
  Q_D(const qColadaH5Item);
  return d->checkState;
}

bool qColadaH5Item::isEvenOneChildChecked() {
  for (int i = 0; i < childCount(); i++) {
    if (getChild(i)->checkState() != Qt::Unchecked)
      return true;
  }
  return false;
}

bool qColadaH5Item::isMapped() const { 
  Q_D(const qColadaH5Item);
  return d->mapped;
}

int qColadaH5Item::getLinkType(){
  Q_D(qColadaH5Item);
  if (!this->isGeoObject() && !this->isGeoContainer())
    return -1;

  if (this->isGeoContainer())
    return static_cast<int>(h5gt::LinkType::Hard);

  if (!d->parentItem)
    return -1;

  std::string objName = data().toStdString();
  if (d->parentItem->isGeoContainer()){
    h5gt::File file = d->parentItem->getGeoContainer()->getH5File();
    if (!file.exist(objName))
      return -1;

    return static_cast<int>(file.getLinkType(objName));
  } else if (d->parentItem->isGeoObject()){
    h5gt::Group group = d->parentItem->getGeoObject()->getObjG();
    if (!group.exist(objName))
      return -1;

    return static_cast<int>(group.getLinkType(objName));
  } else {
    return -1;
  }
}

bool qColadaH5Item::isLinkTypeHard(){
  Q_D(qColadaH5Item);
  return static_cast<h5gt::LinkType>(getLinkType()) == h5gt::LinkType::Hard;
}

bool qColadaH5Item::isLinkTypeSoft(){
  Q_D(qColadaH5Item);
  return static_cast<h5gt::LinkType>(getLinkType()) == h5gt::LinkType::Soft;
}

bool qColadaH5Item::isLinkTypeExternal(){
  Q_D(qColadaH5Item);
  return static_cast<h5gt::LinkType>(getLinkType()) == h5gt::LinkType::External;
}

QString qColadaH5Item::fullName2ItemData(const QString &fullName) {
  return fullName.mid(fullName.lastIndexOf("/") + 1);
}

void qColadaH5Item::write(QDataStream &out) const
{
  Q_D(const qColadaH5Item);
  if (isGeoContainer()) {
    H5BaseContainer *obj = getGeoContainer();
    QString fileName = QString::fromStdString(obj->getH5File().getFileName());
    out << fileName << QString("");
  } else if (isGeoObject()) {
    H5BaseObject *obj = getGeoObject();
    QString fileName = QString::fromStdString(obj->getH5File().getFileName());
    QString objName = QString::fromStdString(obj->getFullName());
    out << fileName << objName;
  }
}

QDataStream &operator<<(QDataStream &out, const qColadaH5Item &item)
{
  item.write(out);
  return out;
}

bool qColadaH5Item::isSame(qColadaH5Item* another){
  return *this == *another;
}

bool qColadaH5Item::operator==(const qColadaH5Item &another) const {
  Q_D(const qColadaH5Item);
  if (isGeoContainer() && another.isGeoContainer())
    return *getGeoContainer() == *another.getGeoContainer();
  else if (isGeoObject() && another.isGeoObject())
    return *getGeoObject() == *another.getGeoObject();

  return false;
}

bool qColadaH5Item::operator!=(const qColadaH5Item &another) const {
  return !(*this == another);
}
