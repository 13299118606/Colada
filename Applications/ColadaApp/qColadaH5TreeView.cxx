// Colada includes
#include "qColadaH5TreeView.h"
#include "qColadaH5TreeView_p.h"
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Model.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QDebug>

// CTK includes
#include <ctkFileDialog.h>

// h5geo includes
#include <h5geo/h5basecontainer.h>

qColadaH5TreeViewPrivate::qColadaH5TreeViewPrivate(qColadaH5TreeView &q)
    : q_ptr(&q) {}

qColadaH5TreeViewPrivate::~qColadaH5TreeViewPrivate() {}

void qColadaH5TreeViewPrivate::init() {
  Q_Q(qColadaH5TreeView);

  q->setSortingEnabled(true);
  q->setSelectionMode(QAbstractItemView::ExtendedSelection);
  //q->setDragEnabled(true);
  //q->setAcceptDrops(true);
  //q->setDropIndicatorShown(true);
  //q->setDragDropMode(QAbstractItemView::InternalMove); // items are moved within tree only
  q->setContextMenuPolicy(Qt::CustomContextMenu);
  q->header()->setContextMenuPolicy(Qt::CustomContextMenu);
  q->setDragDropMode(QAbstractItemView::DragOnly);
  q->setDragEnabled(true);

  qColadaH5ItemDelegate *itDelegate = new qColadaH5ItemDelegate(q);
  q->setItemDelegate(itDelegate);

  qColadaH5ProxyModel* proxy = new qColadaH5ProxyModel(q);
  qColadaH5Model* model = new qColadaH5Model(proxy);
  proxy->setSourceModel(model);

  q->setModel(proxy);

  QObject::connect(q->header(), &QHeaderView::customContextMenuRequested,
                   q, &qColadaH5TreeView::hdrMenuRequested);
  QObject::connect(q, &QTreeView::customContextMenuRequested,
                   q, &qColadaH5TreeView::treeViewMenuRequested);
}

qColadaH5TreeView::qColadaH5TreeView(QWidget *parent)
    : QTreeView(parent), d_ptr(new qColadaH5TreeViewPrivate(*this)) {
  Q_D(qColadaH5TreeView);
  d->init();
}

qColadaH5TreeView::qColadaH5TreeView(qColadaH5TreeViewPrivate *pimpl, QWidget *parent)
    : QTreeView(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qColadaH5TreeView::~qColadaH5TreeView() {}

void qColadaH5TreeView::fillHdrMenu(QMenu *menu, const QPoint &pos) {
  QAction *checkedOnlyAct = menu->addAction("Show checked only");
  checkedOnlyAct->setCheckable(true);

  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (proxyModel)
    checkedOnlyAct->setChecked(proxyModel->isShowCheckedOnly());
  else
    checkedOnlyAct->setDisabled(true);

  menu->addSeparator();

  connect(checkedOnlyAct, &QAction::toggled, proxyModel,
          &qColadaH5ProxyModel::setShowCheckedItemsOnly);

  QAction *expandAllAct = menu->addAction("Expand all");
  connect(expandAllAct, &QAction::triggered, this,
          &QTreeView::expandAll);

  QAction *collapseAllAct = menu->addAction("Collapse all");
  connect(collapseAllAct, &QAction::triggered, this,
          &QTreeView::collapseAll);

  menu->addSeparator();

  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onAddContainer);

  QAction *removeContainerAction = menu->addAction("Remove container");
  connect(removeContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onRemoveContainer);
  QAction *refreshContainerAction = menu->addAction("Refresh container");
  connect(refreshContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onRefreshContainer);

  menu->addSeparator();
}

void qColadaH5TreeView::fillTreeViewMenu(QMenu *menu, const QPoint &pos) {

}

void qColadaH5TreeView::hdrMenuRequested(const QPoint &pos) {
  QMenu *menu = new QMenu(header());

  /* for subclasses to add actions */
  fillHdrMenu(menu, pos);

  //    menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5TreeView::treeViewMenuRequested(const QPoint &pos) {
  QMenu *menu = new QMenu(this);

  /* for subclasses to add actions */
  fillTreeViewMenu(menu, pos);

  //    menu->popup(header()->mapToGlobal(pos));
  menu->exec(this->mapToGlobal(pos));
  menu->deleteLater();
}

bool qColadaH5TreeView::addContainer(const QString &fileName) {
  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (!proxyModel){
    qCritical() << "qColadaH5TreeView::addContainer: proxy model not found";
    return false;
  }

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());

  if (!srcModel){
    qCritical() << "qColadaH5TreeView::addContainer: source model not found";
    return false;
  }

  qColadaH5Item *root = srcModel->getRootItem();
  if (!srcModel->addH5File(fileName)){
    qWarning() << "qColadaH5TreeView::addContainer: unable to add the file: " << fileName;
    return false;
  }
  return true;
}

bool qColadaH5TreeView::removeContainer(const QString &fileName) {
  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (!proxyModel){
    qCritical() << "qColadaH5TreeView::removeContainer: proxy model not found";
    return false;
  }

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());

  if (!srcModel){
    qCritical() << "qColadaH5TreeView::removeContainer: source model not found";
    return false;
  }

  qColadaH5Item *root = srcModel->getRootItem();
  if (!srcModel->removeH5File(fileName)){
    qWarning() << "qColadaH5TreeView::removeContainer: unable to remove the file: " << fileName;
    return false;
  }
  return true;
}

bool qColadaH5TreeView::refreshContainer(const QString &fileName) {
  if (removeContainer(fileName))
    return addContainer(fileName);
  return false;
}

void qColadaH5TreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open Container"), "",
      QObject::tr("hdf5 file (*.h5 *.hdf5); all (*.*)"));

  for (const auto& fileName : h5FileNameList)
    addContainer(fileName);
}

void qColadaH5TreeView::onRemoveContainer() {
  auto* proxy = qobject_cast<qColadaH5ProxyModel*>(this->model());
  auto* srcModel = qobject_cast<qColadaH5Model*>(proxy->sourceModel());
  auto* selectedModel = this->selectionModel();

  if (!selectedModel){
    qCritical() << "qColadaH5TreeView::onRemoveContainer: " <<
                   "selected model is missing";
    return;
  }

  // as we remove file by file we should get all selected names first
  // else we will get incorrect indexes as selected model is not updated
  // each time we remove the row
  QStringList fileNames;
  fileNames.reserve(selectedModel->selectedIndexes().count());
  for (const auto& index : selectedModel->selectedIndexes()){
    QModelIndex srcIndex = proxy->mapToSource(index);
    qColadaH5Item* item = srcModel->itemFromIndex(srcIndex);
    if (!item)
      continue;

    if (!item->isGeoContainer())
      continue;

    fileNames.append(
          QString::fromStdString(
            item->getGeoContainer()->getH5File().getFileName()));
  }

  for (const auto& name : fileNames){
    this->removeContainer(name);
  }
}

void qColadaH5TreeView::onRefreshContainer() {
  auto* proxy = qobject_cast<qColadaH5ProxyModel*>(this->model());
  auto* srcModel = qobject_cast<qColadaH5Model*>(proxy->sourceModel());
  auto* selectedModel = this->selectionModel();

  if (!selectedModel){
    qCritical() << "qColadaH5TreeView::onRefreshContainer: " <<
                   "selected model is missing";
    return;
  }

  for (const auto& index : selectedModel->selectedIndexes()){
    QModelIndex srcIndex = proxy->mapToSource(index);
    qColadaH5Item* item = srcModel->itemFromIndex(srcIndex);
    if (!item)
      continue;

    if (!item->isGeoContainer())
      continue;

    this->refreshContainer(
          QString::fromStdString(
            item->getGeoContainer()->getH5File().getFileName()));
  }
}

