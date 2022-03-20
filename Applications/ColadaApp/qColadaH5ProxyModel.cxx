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
  this->setRecursiveFilteringEnabled(true);
  this->setDynamicSortFilter(true);
}

qColadaH5ProxyModel::~qColadaH5ProxyModel()
{
  delete sourceModel();
}

void qColadaH5ProxyModel::setShowCheckedItemsOnly(bool val) {
  Q_D(qColadaH5ProxyModel);
  d->showCheckedOnlyFlag = val;

  // triggers qColadaH5ProxyModel::filterAcceptsRow()
  invalidateFilter();
}

void qColadaH5ProxyModel::setShowHardLinkItems(bool val){
  Q_D(qColadaH5ProxyModel);
  d->showHardLinks = val;

  // triggers qColadaH5ProxyModel::filterAcceptsRow()
  invalidateFilter();
}

void qColadaH5ProxyModel::setShowSoftLinkItems(bool val){
  Q_D(qColadaH5ProxyModel);
  d->showSoftLinks = val;

  // triggers qColadaH5ProxyModel::filterAcceptsRow()
  invalidateFilter();
}

void qColadaH5ProxyModel::setShowExternalLinkItems(bool val){
  Q_D(qColadaH5ProxyModel);
  d->showExternalLinks = val;

  // triggers qColadaH5ProxyModel::filterAcceptsRow()
  invalidateFilter();
}

bool qColadaH5ProxyModel::isShowCheckedOnly() {
  Q_D(qColadaH5ProxyModel);
  return d->showCheckedOnlyFlag;
}

bool qColadaH5ProxyModel::isShowHardLinkItems() {
  Q_D(qColadaH5ProxyModel);
  return d->showHardLinks;
}

bool qColadaH5ProxyModel::isShowSoftLinkItems() {
  Q_D(qColadaH5ProxyModel);
  return d->showSoftLinks;
}

bool qColadaH5ProxyModel::isShowExternalLinkItems() {
  Q_D(qColadaH5ProxyModel);
  return d->showExternalLinks;
}

bool qColadaH5ProxyModel::filterAcceptsRow(
    int source_row, const QModelIndex &source_parent) const {
  Q_D(const qColadaH5ProxyModel);

  qColadaH5Model *sm = qobject_cast<qColadaH5Model *>(sourceModel());
  if (!sm)
    return false;

  QModelIndex index = sm->index(source_row, 0, source_parent);
  if (!index.isValid())
    return false;

  qColadaH5Item *item = sm->itemFromIndex(index);
  if (item == nullptr)
    return false;

  return filterCheckOnly(item) && filterLinkType(item);
}

bool qColadaH5ProxyModel::filterCheckOnly(qColadaH5Item* item) const
{
  Q_D(const qColadaH5ProxyModel);
  if (!item)
    return false;

  if (d->showCheckedOnlyFlag && !item->checkState())
    return false;

  return true;
}

bool qColadaH5ProxyModel::filterLinkType(qColadaH5Item* item) const
{
  Q_D(const qColadaH5ProxyModel);
  if (!item)
    return false;

  h5gt::LinkType linkType = static_cast<h5gt::LinkType>(item->getLinkType());
  if (!d->showHardLinks && linkType == h5gt::LinkType::Hard){
    return false;
  }
  if (!d->showSoftLinks && linkType == h5gt::LinkType::Soft){
    return false;
  }
  if (!d->showExternalLinks && linkType == h5gt::LinkType::External){
    return false;
  }
  return true;
}
