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

  qColadaH5ItemDelegate *itDelegate = new qColadaH5ItemDelegate(q);
  q->setItemDelegate(itDelegate);

  QObject::connect(q->header(), &QTreeView::customContextMenuRequested,
                   q, &qColadaH5TreeView::hdrMenuRequested);
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

void qColadaH5TreeView::fillHdrMenu(QMenu *menu, QPoint pos) {
  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5TreeView::onAddContainer);
}

void qColadaH5TreeView::hdrMenuRequested(QPoint pos) {
  QMenu *menu = new QMenu(header());

  QAction *checkedOnlyAct = menu->addAction("Show checked only");
  checkedOnlyAct->setCheckable(true);

  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (proxyModel)
    checkedOnlyAct->setChecked(proxyModel->isShowCheckedOnly());
  else
    checkedOnlyAct->setDisabled(true);

  connect(checkedOnlyAct, &QAction::toggled, proxyModel,
          &qColadaH5ProxyModel::setShowCheckedItemsOnly);
  menu->addSeparator();

  /* for subclasses to add actions */
  fillHdrMenu(menu, pos);

  //    menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5TreeView::addContainer(const QString &fileName) {
  this->addContainer(QStringList() << fileName);
}

void qColadaH5TreeView::addContainer(const QStringList &fileNameList) {
  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (!proxyModel)
    return;

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());

  qColadaH5Item *root = srcModel->getRootItem();
  for (int i = 0; i < fileNameList.count(); i++) {
    if (root->getChildByName(fileNameList[i]) == nullptr)
      if (!srcModel->addH5File(fileNameList[i])){
        qWarning() << "qColadaH5TreeView::addContainer(): unable to add the file:" << fileNameList[i];
      }
  }
}

void qColadaH5TreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open Container"), "",
      QObject::tr("hdf5 file (*.h5 *.hdf5); all (*.*)"));

  addContainer(h5FileNameList);
}
