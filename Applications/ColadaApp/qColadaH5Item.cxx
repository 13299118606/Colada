// Colada includes
#include "qColadaH5Item.h"
#include "qColadaH5Item_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>
#include <h5gt/H5DataSet.hpp>

// Qt includes
#include <QStringList>

#include <filesystem>
namespace fs = std::filesystem;

qColadaH5ItemPrivate::qColadaH5ItemPrivate(qColadaH5Item &q) : q_ptr(&q) {}

qColadaH5ItemPrivate::~qColadaH5ItemPrivate() {}

qColadaH5Item::qColadaH5Item(const QString& itemData, qColadaH5Item *parent)
  : d_ptr(new qColadaH5ItemPrivate(*this)) {
  Q_D(qColadaH5Item);

  d->parentItem = parent;
  d->itemData = itemData;
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

void qColadaH5Item::insertChild(qColadaH5Item *item, int position) {
  Q_D(qColadaH5Item);
  if (position < 0)
    position = 0;

  if (position > d->childItems.size())
    position = d->childItems.size();

  d->childItems.insert(position, item);
}

void qColadaH5Item::removeChild(int position, bool destroy) {
  Q_D(qColadaH5Item);
  if (position < 0)
    position = 0;

  if (position > d->childItems.size())
    position = d->childItems.size();

  if (destroy){
    delete d->childItems.takeAt(position);
  } else {
    d->childItems.takeAt(position);
  }
}

bool qColadaH5Item::removeChild(qColadaH5Item *item, bool destroy) {
  if (!item)
    return false;

  int ind = getChildRow(item);
  if (ind < 0)
    return false;

  removeChild(ind, destroy);
  return true;
}

bool qColadaH5Item::removeChildByName(const QString& name, bool destroy) {
  int ind = getChildRow(name);
  if (ind < 0)
    return false;

  removeChild(ind, destroy);
  return true;
}

void qColadaH5Item::removeChildren(int position, int count, bool destroy) {
  Q_D(qColadaH5Item);
  if (position < 0)
    position = 0;

  if (position + count > d->childItems.size())
    count = d->childItems.size() - position;

  for (int i = 0; i < count; ++i){
    if (destroy){
      delete d->childItems.takeAt(position);
    } else {
      d->childItems.takeAt(position);
    }
  }
}

bool qColadaH5Item::hasChild(const QString &name) const {
  Q_D(const qColadaH5Item);
  for (auto &item : d->childItems)
    if (item->data().compare(name, Qt::CaseInsensitive) == 0)
      return true;

  return false;
}

bool qColadaH5Item::hasChildren() const { 
  return childCount() > 0;
}

QString qColadaH5Item::data() const {
  Q_D(const qColadaH5Item);
  return d->itemData;
}

bool qColadaH5Item::setData(const QString &newItemData) {
  Q_D(qColadaH5Item);
  if (isRoot()){
    d->itemData = newItemData;
    return true;
  }

  if (isContainer())
    return false;

  qColadaH5Item* parent = getParent();
  if (!parent)
    return false;

  auto opt = parent->getObjG();
  if (opt.has_value())
    return false;

  QString oldItemData = this->data();
  if (!opt->hasObject(oldItemData.toStdString(), h5gt::ObjectType::Group) ||
      opt->exist(newItemData.toStdString()))
    return false;

  opt->rename(oldItemData.toStdString(), newItemData.toStdString());
  opt->flush();
  return true;
}

int qColadaH5Item::getChildRow(qColadaH5Item *item)
{
  Q_D(qColadaH5Item);
  return d->childItems.indexOf(item);
}

int qColadaH5Item::getChildRow(const QString& name)
{
  Q_D(qColadaH5Item);
  for (int i = 0; i < d->childItems.count(); i++){
    if (d->childItems[i]->data() == name)
      return i;
  }
  return -1;
}

QString qColadaH5Item::getPath() const{
  Q_D(const qColadaH5Item);
  if (isRoot())
    return QString();

  if (isContainer())
    return QString("/");

  QString objPath = d->itemData;
  qColadaH5Item* p = getParent();
  while (p && !p->isContainer() && !p->isRoot()) {
    objPath.prepend(p->data() + "/");
    p = getParent();
  }

  return objPath;
}

qColadaH5Item *qColadaH5Item::getContainerItem() const
{
  if (this->isContainer())
    return const_cast<qColadaH5Item*>(this);

  qColadaH5Item* p = getParent();
  while (p) {
    if (p->isContainer())
      return p;
    p = getParent();
  }
  return nullptr;
}

std::optional<h5gt::File> qColadaH5Item::getH5File() const
{
  Q_D(const qColadaH5Item);
  if (isRoot())
    return std::nullopt;

  if (this->isContainer()){
    if (!fs::exists(d->itemData.toStdString()) ||
        H5Fis_hdf5(d->itemData.toStdString().c_str()) <= 0)
      return std::nullopt;

    try {
      h5gt::File h5File(
            d->itemData.toStdString(),
            h5gt::File::ReadWrite);

      return h5File;
    } catch (h5gt::Exception& err) {
      return std::nullopt;
    }
  }

  qColadaH5Item* cntItem = this->getContainerItem();
  if (!cntItem)
    return std::nullopt;
  return cntItem->getH5File();
}

std::optional<h5gt::Group> qColadaH5Item::getObjG() const
{
  Q_D(const qColadaH5Item);
  auto opt = getH5File();
  if (!opt.has_value())
    return std::nullopt;

  QString objPath = this->getPath();
  if (!objPath.isEmpty() &&
      opt->hasObject(objPath.toStdString(), h5gt::ObjectType::Group))
    return opt->getGroup(objPath.toStdString());

  return std::nullopt;
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
  qColadaH5Item *item = this;
  while (item) {
    itemListToRoot.prepend(item);
    item = item->getParent();
  }
  return itemListToRoot;
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
  return !d->parentItem;
}

bool qColadaH5Item::isContainer() const{
  Q_D(const qColadaH5Item);
  if (!isRoot() &&
      getParent() &&
      getParent()->isRoot())
    return true;

  return false;
}

bool qColadaH5Item::isObject() const{
  auto opt = getH5File();
  if (!opt.has_value())
    return false;

  QString objPath = getPath();
  if (!objPath.isEmpty() || objPath == "/")
    return false;

  return opt->hasObject(objPath.toStdString(), h5gt::ObjectType::Group);
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
  QString objPath = getPath();
  if (objPath.isEmpty())
    return -1;

  if (objPath == "/")
    return static_cast<int>(h5gt::LinkType::Hard);

  auto opt = getH5File();
  if (!opt.has_value() ||
      opt->hasObject(objPath.toStdString(), h5gt::ObjectType::Group))
    return -1;

  return static_cast<int>(opt->getLinkType(objPath.toStdString()));
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

void qColadaH5Item::write(QDataStream &out) const
{
  Q_D(const qColadaH5Item);
  auto opt = getH5File();
  if (!opt.has_value())
    return;

  out << QString::fromStdString(opt->getFileName()) << this->getPath();
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
  if (this->isRoot() && another.isRoot())
    return this->data() == another.data();

  auto thisFile_opt = getH5File();
  auto anotherFile_opt = another.getH5File();
  if (!thisFile_opt.has_value() || !anotherFile_opt.has_value())
    return false;

  if (thisFile_opt.value() != anotherFile_opt.value())
    return false;

  return this->getPath() == another.getPath();
}

bool qColadaH5Item::operator!=(const qColadaH5Item &another) const {
  return !(*this == another);
}
