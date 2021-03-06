// Colada includes
#include "qColadaH5TreeView.h"
#include "qColadaH5TreeView_p.h"
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Model.h"
#include "qColadaAppMainWindow.h"

// Slicer includes
#include "qSlicerApplication.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QDebug>
#include <QDragEnterEvent>

// CTK includes
#include <ctkFileDialog.h>

// h5geo includes
#include <h5geo/h5basecontainer.h>

qColadaH5TreeViewPrivate::qColadaH5TreeViewPrivate(qColadaH5TreeView &q)
    : q_ptr(&q) {}

qColadaH5TreeViewPrivate::~qColadaH5TreeViewPrivate() {}

void qColadaH5TreeViewPrivate::init() {
  Q_Q(qColadaH5TreeView);

  app = qSlicerApplication::application();
  if (!app){
    qCritical() << Q_FUNC_INFO << ": Unable to get application instance";
    return;
  }

  mainW = qobject_cast<qColadaAppMainWindow*>(app->mainWindow());
  if (!mainW){
    qCritical() << Q_FUNC_INFO << ": Unable to get mainwindow instance";
    return;
  }

  q->setSortingEnabled(true);
  q->setSelectionMode(QAbstractItemView::ExtendedSelection);
  q->setContextMenuPolicy(Qt::CustomContextMenu);
  q->header()->setContextMenuPolicy(Qt::CustomContextMenu);
  q->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
  q->setDragEnabled(true);
  q->setAcceptDrops(true);
  q->setDropIndicatorShown(true);

//  q->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());

  QAction *checkedOnlyAct = menu->addAction("Show Checked only");
  checkedOnlyAct->setCheckable(true);
  checkedOnlyAct->setChecked(proxyModel->isShowCheckedOnly());
  connect(checkedOnlyAct, &QAction::toggled,
          proxyModel, &qColadaH5ProxyModel::setShowCheckedItemsOnly);
  menu->addSeparator();

  QAction *hardLinksAct = menu->addAction("Show Hard links");
  hardLinksAct->setCheckable(true);
  hardLinksAct->setChecked(proxyModel->isShowHardLinkItems());
  connect(hardLinksAct, &QAction::toggled,
          proxyModel, &qColadaH5ProxyModel::setShowHardLinkItems);
  QAction *softLinksAct = menu->addAction("Show Soft links");
  softLinksAct->setCheckable(true);
  softLinksAct->setChecked(proxyModel->isShowSoftLinkItems());
  connect(softLinksAct, &QAction::toggled,
          proxyModel, &qColadaH5ProxyModel::setShowSoftLinkItems);
  QAction *externalLinksAct = menu->addAction("Show External links");
  externalLinksAct->setCheckable(true);
  externalLinksAct->setChecked(proxyModel->isShowExternalLinkItems());
  connect(externalLinksAct, &QAction::toggled,
          proxyModel, &qColadaH5ProxyModel::setShowExternalLinkItems);
  menu->addSeparator();

  QAction *expandSelectedAct = menu->addAction("Expand");
  connect(expandSelectedAct, &QAction::triggered,
          this, &qColadaH5TreeView::onExpandSelected);

  QAction *collapseSelectedAct = menu->addAction("Collapse");
  connect(collapseSelectedAct, &QAction::triggered,
          this, &qColadaH5TreeView::onCollapseSelected);

  QAction *expandAllAct = menu->addAction("Expand all");
  connect(expandAllAct, &QAction::triggered,
          this, &QTreeView::expandAll);

  QAction *collapseAllAct = menu->addAction("Collapse all");
  connect(collapseAllAct, &QAction::triggered,
          this, &QTreeView::collapseAll);
  menu->addSeparator();
}

void qColadaH5TreeView::fillTreeViewMenu(QMenu *menu, const QPoint &pos) {
  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onAddContainer);

  QAction *removeContainerAction = menu->addAction("Remove container");
  connect(removeContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onRemoveContainer);
  QAction *refetchObjectAction = menu->addAction("Refetch object");
  connect(refetchObjectAction, &QAction::triggered, this,
          &qColadaH5TreeView::onRefetchObject);
  menu->addSeparator();

  QAction *renameAction = menu->addAction("Rename object");
  connect(renameAction, &QAction::triggered, this,
          &qColadaH5TreeView::onRenameObject);
  QAction *unlinkAction = menu->addAction("Unlink object");
  connect(unlinkAction, &QAction::triggered, this,
          &qColadaH5TreeView::onUnlinkObject);
  menu->addSeparator();
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
    qCritical() << Q_FUNC_INFO << ": Proxy model not found";
    return false;
  }

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());
  if (!srcModel){
    qCritical() << Q_FUNC_INFO << ": Source model not found";
    return false;
  }

  qColadaH5Item *root = srcModel->getRootItem();
  if (!srcModel->addH5File(fileName)){
    qWarning() << Q_FUNC_INFO << ": Unable to add the file: " << fileName;
    return false;
  }
  return true;
}

bool qColadaH5TreeView::removeContainer(const QString &fileName) {
  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (!proxyModel){
    qCritical() << Q_FUNC_INFO << ": Proxy model not found";
    return false;
  }

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());

  if (!srcModel){
    qCritical() << Q_FUNC_INFO << ": Source model not found";
    return false;
  }

  qColadaH5Item *root = srcModel->getRootItem();
  if (!srcModel->removeH5File(fileName)){
    qInfo() << Q_FUNC_INFO << ": Unable to remove the file: " << fileName;
    return false;
  }
  return true;
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
  auto* selectionModel = this->selectionModel();
  if (!selectionModel){
    qCritical() << Q_FUNC_INFO << ": Selected model is missing";
    return;
  }

  QModelIndexList indexes = selectionModel->selectedIndexes();
  if (indexes.isEmpty())
    indexes.append(currentIndex());

  // as we remove file by file we should get all selected names first
  // or we will get incorrect indexes as selected model is not updated
  // each time we remove the row
  // Remeber: item may be invalid i.e. File or Group is unavailable but still
  // it must be removed by click
  QStringList fileNames;
  fileNames.reserve(indexes.count());
  for (const auto& index : indexes){
    QModelIndex srcIndex = proxy->mapToSource(index);
    qColadaH5Item* item = srcModel->itemFromIndex(srcIndex);
    if (!item)
      continue;

    fileNames.append(item->rawData());
  }

  for (const auto& name : fileNames){
    this->removeContainer(name);
  }
}

void qColadaH5TreeView::onRefetchObject() {
  auto* proxy = qobject_cast<qColadaH5ProxyModel*>(this->model());
  auto* srcModel = qobject_cast<qColadaH5Model*>(proxy->sourceModel());
  auto* selectionModel = this->selectionModel();
  if (!selectionModel){
    qCritical() << Q_FUNC_INFO << ": Selected model is missing";
    return;
  }

  QModelIndexList indexes = selectionModel->selectedIndexes();
  if (indexes.isEmpty())
    indexes.append(currentIndex());

  for (const auto& index : indexes){
    srcModel->refetch(proxy->mapToSource(index));
  }
}

void qColadaH5TreeView::onRenameObject(){
  Q_D(qColadaH5TreeView);
  this->edit(currentIndex());
}

void qColadaH5TreeView::onUnlinkObject() {
  auto* proxy = qobject_cast<qColadaH5ProxyModel*>(this->model());
  auto* srcModel = qobject_cast<qColadaH5Model*>(proxy->sourceModel());
  auto* selectionModel = this->selectionModel();
  if (!selectionModel){
    qCritical() << Q_FUNC_INFO << ": Selected model is missing";
    return;
  }

  QModelIndexList indexes = selectionModel->selectedIndexes();
  if (indexes.isEmpty())
    indexes.append(currentIndex());

  for (const auto& index : indexes){
    QModelIndex srcIndex = proxy->mapToSource(index);
    qColadaH5Item* item = srcModel->itemFromIndex(srcIndex);
    if (!item)
      continue;

    srcModel->removeItem(item, true);
  }
}

void qColadaH5TreeView::onExpandSelected() {
  auto* selectionModel = this->selectionModel();
  if (!selectionModel){
    qCritical() << Q_FUNC_INFO << ": Selected model is missing";
    return;
  }

  QModelIndexList indexes = selectionModel->selectedIndexes();
  if (indexes.isEmpty())
    indexes.append(currentIndex());

  for (const auto& index : indexes){
    this->expandRecursively(index, -1);
  }
}

void qColadaH5TreeView::onCollapseSelected() {
  auto* selectionModel = this->selectionModel();
  if (!selectionModel){
    qCritical() << Q_FUNC_INFO << ": Selected model is missing";
    return;
  }

  QModelIndexList indexes = selectionModel->selectedIndexes();
  if (indexes.isEmpty())
    indexes.append(currentIndex());

  for (const auto& index : indexes){
    this->collapse(index);
  }
}

void qColadaH5TreeView::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source() == this)
    event->acceptProposedAction();
}
