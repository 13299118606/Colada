// Colada includes
#include "qColadaH5SurfTreeView.h"
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5SurfModel.h"
#include "qColadaH5SurfTreeView_p.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>

// CTK includes
#include <ctkFileDialog.h>

qColadaH5SurfTreeViewPrivate::qColadaH5SurfTreeViewPrivate(
    qColadaH5SurfTreeView &q)
    : Superclass(q) {}

qColadaH5SurfTreeViewPrivate::~qColadaH5SurfTreeViewPrivate() {}

void qColadaH5SurfTreeViewPrivate::init() {
  Q_Q(qColadaH5SurfTreeView);
  this->Superclass::init();
}

qColadaH5SurfTreeView::qColadaH5SurfTreeView(QWidget *parent)
    : qColadaH5TreeView(new qColadaH5SurfTreeViewPrivate(*this), parent) {
  Q_D(qColadaH5SurfTreeView);
  d->init();
}

qColadaH5SurfTreeView::qColadaH5SurfTreeView(
    qColadaH5SurfTreeViewPrivate *pimpl, QWidget *parent)
    : qColadaH5TreeView(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5SurfTreeView::~qColadaH5SurfTreeView() {}

void qColadaH5SurfTreeView::fillHdrMenu(QMenu *menu, QPoint pos) {
  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5SurfTreeView::onAddContainer);
}

void qColadaH5SurfTreeView::hdrMenuRequested(QPoint pos) {
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

  // menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5SurfTreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open surface container"), "",
      QObject::tr("hdf5 file (*.h5 *.hdf5); all (*.*)"));

  qColadaH5ProxyModel *proxyModel =
      qobject_cast<qColadaH5ProxyModel *>(model());
  if (!proxyModel)
    return;

  qColadaH5Model *srcModel =
      static_cast<qColadaH5Model *>(proxyModel->sourceModel());

  for (int i = 0; i < h5FileNameList.count(); i++) {
    srcModel->addH5File(h5FileNameList[i]);
  }
}