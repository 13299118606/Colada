// Colada includes
#include "qColadaH5MapModel.h"
#include "qColadaH5MapModel_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5mapcontainer.h>
#include <h5geo/h5map.h>

qColadaH5MapModelPrivate::qColadaH5MapModelPrivate(qColadaH5MapModel &q)
    : Superclass(q) {}

//-----------------------------------------------------------------------------
qColadaH5MapModelPrivate::~qColadaH5MapModelPrivate() {}

void qColadaH5MapModelPrivate::init(const QString &title) {
  Q_Q(qColadaH5MapModel);
  this->Superclass::init(title);
}

qColadaH5MapModel::qColadaH5MapModel(QObject *parent)
    : Superclass(new qColadaH5MapModelPrivate(*this), parent) {
  Q_D(qColadaH5MapModel);
  d->init("");
}

qColadaH5MapModel::qColadaH5MapModel(const QString &title, QObject *parent) 
  : Superclass(new qColadaH5MapModelPrivate(*this), parent) {
  Q_D(qColadaH5MapModel);
  d->init(title);
}

qColadaH5MapModel::qColadaH5MapModel(
    qColadaH5MapModelPrivate *pimpl, QObject *parent)
    : Superclass(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5MapModel::~qColadaH5MapModel() {}

QVariant qColadaH5MapModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole) 
    if (mapFromItem(itemFromIndex(index)) == nullptr)
      return QVariant();

  return qColadaH5Model::data(index, role);
}

bool qColadaH5MapModel::hasChildren(const QModelIndex& parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  H5Map* map = mapFromItem(parentItem);
  if (map)
    return false;

  return qColadaH5Model::hasChildren(parent);
}

void qColadaH5MapModel::fetchMore(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);

  QVector<qColadaH5Item *> childItems;
  if (parentItem->isGeoContainer()) {
    H5BaseContainer *obj = parentItem->getGeoContainer();
    if (obj == nullptr)
      return;

    h5gt::File file = obj->getH5File();
    std::vector<std::string> childrenNameList = file.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (file.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = file.getGroup(childrenNameList[i]);
      if (h5geo::isMap(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openMap(group), parentItem));
      } else {
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openBaseObject(group), parentItem));
      }
    }
  } else if (parentItem->isGeoObject()) {
    H5BaseObject *obj = parentItem->getGeoObject();
    if (obj == nullptr ||
        dynamic_cast<H5Map *>(obj))
      return;

    h5gt::Group objG = obj->getObjG();
    std::vector<std::string> childrenNameList = objG.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (objG.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = objG.getGroup(childrenNameList[i]);
      if (h5geo::isMap(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openMap(group), parentItem));
      } else {
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openBaseObject(group), parentItem));
      }
    }
  }
  childItems.shrink_to_fit();

  if (childItems.isEmpty())
    return;

  int insrtCnt = childItems.count() - 1;
  beginInsertRows(parent, 0, insrtCnt);
  parentItem->setChildren(childItems);
  endInsertRows();
  parentItem->setMapped(true);
}

Qt::ItemFlags qColadaH5MapModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags =
      Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
  
  if (mapFromItem(itemFromIndex(index)))
    flags |= Qt::ItemIsUserCheckable;

  return flags;
}

bool qColadaH5MapModel::canAddH5File(h5gt::File file) const { 
  H5MapCnt_ptr mapCnt(h5geo::openMapContainer(file));
  if (mapCnt == nullptr)
    return false;
  return true;
}

H5Map *qColadaH5MapModel::mapFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5Map *>(item->getGeoObject());
}

H5MapContainer *
qColadaH5MapModel::mapCntFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5MapContainer *>(item->getGeoContainer());
}
