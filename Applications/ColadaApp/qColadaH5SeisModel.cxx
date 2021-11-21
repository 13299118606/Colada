// Colada includes
#include "qColadaH5SeisModel.h"
#include "qColadaH5SeisModel_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5seis.h>
#include <h5geo/h5seiscontainer.h>

qColadaH5SeisModelPrivate::qColadaH5SeisModelPrivate(qColadaH5SeisModel &q)
    : Superclass(q) {}

//-----------------------------------------------------------------------------
qColadaH5SeisModelPrivate::~qColadaH5SeisModelPrivate() {}

void qColadaH5SeisModelPrivate::init(const QString &title) {
  Q_Q(qColadaH5SeisModel);
  this->Superclass::init(title);
}

qColadaH5SeisModel::qColadaH5SeisModel(QObject *parent)
    : Superclass(new qColadaH5SeisModelPrivate(*this), parent) {
  Q_D(qColadaH5SeisModel);
  d->init("");
}

qColadaH5SeisModel::qColadaH5SeisModel(const QString &title, QObject *parent)
    : Superclass(new qColadaH5SeisModelPrivate(*this), parent) {
  Q_D(qColadaH5SeisModel);
  d->init(title);
}

qColadaH5SeisModel::qColadaH5SeisModel(qColadaH5SeisModelPrivate *pimpl,
                                       QObject *parent)
    : Superclass(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5SeisModel::~qColadaH5SeisModel() {}

QVariant qColadaH5SeisModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole)
    if (seisFromItem(itemFromIndex(index)) == nullptr)
      return QVariant();

  return qColadaH5Model::data(index, role);
}

bool qColadaH5SeisModel::hasChildren(const QModelIndex &parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  H5Seis *seis = seisFromItem(parentItem);
  if (seis)
    return false;

  return qColadaH5Model::hasChildren(parent);
}

void qColadaH5SeisModel::fetchMore(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);

  QVector<qColadaH5Item *> childItems;
  if (parentItem->isGeoContainer()) {
    H5BaseContainer *obj = parentItem->getGeoContainer();
    h5gt::File file = obj->getH5File();
    H5SeisCnt_ptr seisCnt(h5geo::openSeisContainer(file));
    if (seisCnt == nullptr)
      return;

    std::vector<std::string> childrenNameList = file.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (file.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = file.getGroup(childrenNameList[i]);
      H5Seis *seis = seisCnt->getSeis(group);
      if (seis) {
        childItems.push_back(new qColadaH5Item(seis, parentItem));
      } else {
        H5BaseObject *baseObj = h5geo::openBaseObject(group);
        childItems.push_back(new qColadaH5Item(baseObj, parentItem));
      }
    }
  } else if (parentItem->isGeoObject()) {
    H5BaseObject *obj = parentItem->getGeoObject();
    if (obj == nullptr || dynamic_cast<H5Seis *>(obj))
      return;

    h5gt::File file = obj->getH5File();
    h5gt::Group objG = obj->getObjG();

    H5SeisCnt_ptr seisCnt(h5geo::openSeisContainer(file));
    if (seisCnt == nullptr)
      return;

    std::vector<std::string> childrenNameList = objG.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (objG.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = objG.getGroup(childrenNameList[i]);
      H5Seis *seis = seisCnt->getSeis(group);
      if (seis) {
        childItems.push_back(new qColadaH5Item(seis, parentItem));
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

Qt::ItemFlags qColadaH5SeisModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags =
      Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;

  if (seisFromItem(itemFromIndex(index)))
    flags |= Qt::ItemIsUserCheckable;

  return flags;
}

bool qColadaH5SeisModel::canAddH5File(h5gt::File file) const {
  H5SeisCnt_ptr seisCnt(h5geo::openSeisContainer(file));
  if (seisCnt == nullptr)
    return false;
  return true;
}

H5Seis *qColadaH5SeisModel::seisFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5Seis *>(item->getGeoObject());
}

H5SeisContainer *
qColadaH5SeisModel::seisCntFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5SeisContainer *>(item->getGeoContainer());
}
