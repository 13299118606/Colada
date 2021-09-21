// Colada includes
#include "qColadaH5SurfModel.h"
#include "qColadaH5SurfModel_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5surfcontainer.h>
#include <h5geo/h5surf.h>

qColadaH5SurfModelPrivate::qColadaH5SurfModelPrivate(qColadaH5SurfModel &q)
    : Superclass(q) {}

//-----------------------------------------------------------------------------
qColadaH5SurfModelPrivate::~qColadaH5SurfModelPrivate() {}

void qColadaH5SurfModelPrivate::init(QString title) {
  Q_Q(qColadaH5SurfModel);
  this->Superclass::init(title);
}

qColadaH5SurfModel::qColadaH5SurfModel(QString title, QObject *parent) 
  : Superclass(new qColadaH5SurfModelPrivate(*this), parent) {
  Q_D(qColadaH5SurfModel);
  d->init(title);
}

qColadaH5SurfModel::qColadaH5SurfModel(
    qColadaH5SurfModelPrivate *pimpl, QObject *parent)
    : Superclass(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5SurfModel::~qColadaH5SurfModel() {}

QVariant qColadaH5SurfModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole) 
    if (surfFromItem(itemFromIndex(index)) == nullptr)
      return QVariant();

  return qColadaH5Model::data(index, role);
}

bool qColadaH5SurfModel::hasChildren(const QModelIndex& parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  H5Surf* surf = surfFromItem(parentItem);
  if (surf)
    return false;

  return qColadaH5Model::hasChildren(parent);
}

void qColadaH5SurfModel::fetchMore(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);

  QVector<qColadaH5Item *> childItems;
  if (parentItem->isGeoContainer()) {
    H5BaseContainer *obj = parentItem->getGeoContainer();
    h5gt::File file = obj->getH5File();
    H5SurfCnt_ptr surfCnt(h5geo::openSurfContainer(file));
    if (surfCnt == nullptr)
      return;

    std::vector<std::string> childrenNameList = file.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (file.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = file.getGroup(childrenNameList[i]);
      H5Surf *surf = surfCnt->getSurf(group);
      if (surf) {
        childItems.push_back(new qColadaH5Item(surf, parentItem));
      } else {
        H5BaseObject *baseObj = h5geo::openBaseObject(group);
        childItems.push_back(new qColadaH5Item(baseObj, parentItem));
      }
    }
  } else if (parentItem->isGeoObject()) {
    H5BaseObject *obj = parentItem->getGeoObject();
    if (obj == nullptr || dynamic_cast<H5Surf *>(obj))
      return;

    h5gt::File file = obj->getH5File();
    h5gt::Group objG = obj->getObjG();

    H5SurfCnt_ptr surfCnt(h5geo::openSurfContainer(file));
    if (surfCnt == nullptr)
      return;

    std::vector<std::string> childrenNameList = objG.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (objG.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = objG.getGroup(childrenNameList[i]);
      H5Surf *surf = surfCnt->getSurf(group);
      if (surf) {
        childItems.push_back(new qColadaH5Item(surf, parentItem));
      } else {
        H5BaseObject *baseObj = h5geo::openBaseObject(group);
        childItems.push_back(new qColadaH5Item(baseObj, parentItem));
      }
    }
  }
  childItems.shrink_to_fit();

  if (childItems.isEmpty())
    return;

  int insrtCnt = childItems.count() - 1;
  beginInsertRows(parent, 0, insrtCnt);
  parentItem->setChildItems(childItems);
  endInsertRows();
  parentItem->setMapped(true);
}

Qt::ItemFlags qColadaH5SurfModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled |
                        Qt::ItemIsEditable;
  
  if (surfFromItem(itemFromIndex(index)))
    flags |= Qt::ItemIsUserCheckable;

  return flags;
}

bool qColadaH5SurfModel::canAddH5File(h5gt::File file) const { 
  H5SurfCnt_ptr surfCnt(h5geo::openSurfContainer(file));
  if (surfCnt == nullptr)
    return false;
  return true;
}

H5Surf *qColadaH5SurfModel::surfFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5Surf *>(item->getGeoObject());
}

H5SurfContainer *
qColadaH5SurfModel::surfCntFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5SurfContainer *>(item->getGeoContainer());
}