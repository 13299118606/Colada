// Colada includes
#include "qColadaH5WellModel.h"
#include "qColadaH5WellModel_p.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5well.h>
#include <h5geo/h5wellcontainer.h>
#include <h5geo/h5devcurve.h>
#include <h5geo/h5logcurve.h>

qColadaH5WellModelPrivate::qColadaH5WellModelPrivate(qColadaH5WellModel &q)
    : Superclass(q) {}

//-----------------------------------------------------------------------------
qColadaH5WellModelPrivate::~qColadaH5WellModelPrivate() {}

void qColadaH5WellModelPrivate::init(const QString &title) {
  Q_Q(qColadaH5WellModel);
  this->Superclass::init(title);
}

qColadaH5WellModel::qColadaH5WellModel(QObject *parent)
    : Superclass(new qColadaH5WellModelPrivate(*this), parent) {
  Q_D(qColadaH5WellModel);
  d->init("");
}

qColadaH5WellModel::qColadaH5WellModel(const QString &title, QObject *parent)
    : Superclass(new qColadaH5WellModelPrivate(*this), parent) {
  Q_D(qColadaH5WellModel);
  d->init(title);
}

qColadaH5WellModel::qColadaH5WellModel(qColadaH5WellModelPrivate *pimpl,
                                       QObject *parent)
    : Superclass(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5WellModel::~qColadaH5WellModel() {}

QVariant qColadaH5WellModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole)
    if (wellFromItem(itemFromIndex(index)) == nullptr &&
        logCurveFromItem(itemFromIndex(index)) == nullptr)
      return QVariant();

  return qColadaH5Model::data(index, role);
}

bool qColadaH5WellModel::hasChildren(const QModelIndex &parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  H5DevCurve *devCurve = devCurveFromItem(parentItem);
  if (devCurve)
    return false;

  H5LogCurve *logCurve = logCurveFromItem(parentItem);
  if (logCurve)
    return false;

  return qColadaH5Model::hasChildren(parent);
}

void qColadaH5WellModel::fetchMore(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);

  QVector<qColadaH5Item *> childItems;
  if (parentItem->isGeoContainer()) {
    H5BaseContainer *obj = parentItem->getGeoContainer();
    h5gt::File file = obj->getH5File();
    H5WellCnt_ptr wellCnt(h5geo::openWellContainer(file));
    if (wellCnt == nullptr)
      return;

    std::vector<std::string> childrenNameList = file.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (file.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = file.getGroup(childrenNameList[i]);
      H5Well *well = wellCnt->getWell(group);
      if (well) {
        childItems.push_back(new qColadaH5Item(well, parentItem));
      } else {
        H5BaseObject *baseObj = h5geo::openBaseObject(group);
        childItems.push_back(new qColadaH5Item(baseObj, parentItem));
      }
    }
  } else if (parentItem->isGeoObject()) {
    H5BaseObject *obj = parentItem->getGeoObject();
    if (obj == nullptr || dynamic_cast<H5DevCurve *>(obj) ||
        dynamic_cast<H5LogCurve *>(obj))
      return;

    h5gt::File file = obj->getH5File();
    h5gt::Group objG = obj->getObjG();
    h5gt::Group parentG = objG;

    H5WellCnt_ptr wellCnt(h5geo::openWellContainer(file));
    if (wellCnt == nullptr)
      return;

    H5Well_ptr well(nullptr);

    while (parentG.getPath() != "/") {
      well = H5Well_ptr(wellCnt->getWell(parentG));
      if (well)
        break;

      parentG = parentG.getParent();
    }

    std::vector<std::string> childrenNameList = objG.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (objG.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = objG.getGroup(childrenNameList[i]);

      if (well == nullptr) {
        H5Well *well = wellCnt->getWell(group);
        if (well) {
          childItems.push_back(new qColadaH5Item(well, parentItem));
        } else {
          H5BaseObject *baseObj = h5geo::openBaseObject(group);
          childItems.push_back(new qColadaH5Item(baseObj, parentItem));
        }
        continue;
      }

      H5DevCurve *devCurve = well->getDevCurve(group);
      if (devCurve) {
        childItems.push_back(new qColadaH5Item(devCurve, parentItem));
        continue;
      }

      H5LogCurve *logCurve = well->getLogCurve(group);
      if (logCurve) {
        childItems.push_back(new qColadaH5Item(logCurve, parentItem));
        continue;
      }

      H5BaseObject *baseObj = h5geo::openBaseObject(group);
      childItems.push_back(new qColadaH5Item(baseObj, parentItem));
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

Qt::ItemFlags qColadaH5WellModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags =
      Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;

  qColadaH5Item *item = itemFromIndex(index);
  if (wellFromItem(item) || logCurveFromItem(item))
    flags |= Qt::ItemIsUserCheckable;

  return flags;
}

bool qColadaH5WellModel::canAddH5File(h5gt::File file) const {
  H5WellCnt_ptr wellCnt(h5geo::openWellContainer(file));
  if (wellCnt == nullptr)
    return false;
  return true;
}

H5Well *qColadaH5WellModel::wellFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5Well *>(item->getGeoObject());
}

H5WellContainer *
qColadaH5WellModel::wellCntFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5WellContainer *>(item->getGeoContainer());
}

H5DevCurve *qColadaH5WellModel::devCurveFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5DevCurve *>(item->getGeoObject());
}

H5LogCurve *qColadaH5WellModel::logCurveFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5LogCurve *>(item->getGeoObject());
}