// Colada includes
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Item.h"
#include "qColadaH5Model.h"
#include "qColadaH5ProxyModel_p.h"

qColadaH5ProxyModelPrivate::qColadaH5ProxyModelPrivate(qColadaH5ProxyModel &q)
    : q_ptr(&q) {}

qColadaH5ProxyModelPrivate::~qColadaH5ProxyModelPrivate() {}

qColadaH5ProxyModel::qColadaH5ProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      d_ptr(new qColadaH5ProxyModelPrivate(*this)) {}

qColadaH5ProxyModel::~qColadaH5ProxyModel() { delete sourceModel(); }

void qColadaH5ProxyModel::setShowCheckedItemsOnly(bool val) {
  Q_D(qColadaH5ProxyModel);

  d->showCheckedOnlyFlag = val;
  qColadaH5Model *sm = static_cast<qColadaH5Model *>(sourceModel());
  sm->sendAllChildDataChanged(sm->getRootItem());
}

bool qColadaH5ProxyModel::isShowCheckedOnly() {
  Q_D(qColadaH5ProxyModel);
  return d->showCheckedOnlyFlag;
}

bool qColadaH5ProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {
  Q_D(const qColadaH5ProxyModel);
  if (!d->showCheckedOnlyFlag) {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    //        if (!index.isValid())
    //            return false;

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
  } else {
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if (!index.isValid())
      return false;

    qColadaH5Item *item = static_cast<qColadaH5Item *>(index.internalPointer());
    if (item == nullptr)
      return false;

    if (item->isRoot())
      return true;

    return item->checkState() != Qt::Unchecked;
  }
}
