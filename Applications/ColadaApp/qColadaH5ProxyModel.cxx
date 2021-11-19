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
      d_ptr(new qColadaH5ProxyModelPrivate(*this))
{
  this->setRecursiveFilteringEnabled(false);
}

qColadaH5ProxyModel::~qColadaH5ProxyModel()
{
  delete sourceModel();
}

void qColadaH5ProxyModel::setShowCheckedItemsOnly(bool val) {
  Q_D(qColadaH5ProxyModel);

  d->showCheckedOnlyFlag = val;

  // triggers qColadaH5ProxyModel::filterAcceptsRow()
  invalidate();
}

bool qColadaH5ProxyModel::isShowCheckedOnly() {
  Q_D(qColadaH5ProxyModel);
  return d->showCheckedOnlyFlag;
}

bool qColadaH5ProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {
  Q_D(const qColadaH5ProxyModel);

  if (d->showCheckedOnlyFlag){
    qColadaH5Model *sm = qobject_cast<qColadaH5Model *>(sourceModel());
    if (!sm)
      return false;

    QModelIndex index = sm->index(source_row, 0, source_parent);
    if (!index.isValid())
      return false;

    qColadaH5Item *item = sm->itemFromIndex(index);
    if (item == nullptr)
      return false;

    if (item->checkState())
      return true;

    // if item is not checked but some of its child is checked then return true
    QList<qColadaH5Item*> itemList;
    sm->getFullChildList(item, itemList);
    for (const qColadaH5Item* it : itemList){
      if (it->checkState())
        return true;
    }
    return false;
  } else {
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
  }
}
