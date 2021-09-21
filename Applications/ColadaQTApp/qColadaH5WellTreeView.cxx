// Colada includes
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5WellModel.h"
#include "qColadaH5WellTreeView.h"
#include "qColadaH5WellTreeView_p.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>

// CTK includes
#include <ctkFileDialog.h>

qColadaH5WellTreeViewPrivate::qColadaH5WellTreeViewPrivate(
    qColadaH5WellTreeView &q)
    : Superclass(q) {}

qColadaH5WellTreeViewPrivate::~qColadaH5WellTreeViewPrivate() {}

void qColadaH5WellTreeViewPrivate::init() {
  Q_Q(qColadaH5WellTreeView);
  this->Superclass::init();
}

qColadaH5WellTreeView::qColadaH5WellTreeView(QWidget *parent)
    : qColadaH5TreeView(new qColadaH5WellTreeViewPrivate(*this), parent) {
  Q_D(qColadaH5WellTreeView);
  d->init();
}

qColadaH5WellTreeView::qColadaH5WellTreeView(
    qColadaH5WellTreeViewPrivate *pimpl, QWidget *parent)
    : qColadaH5TreeView(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5WellTreeView::~qColadaH5WellTreeView() {}

void qColadaH5WellTreeView::fillHdrMenu(QMenu *menu, QPoint pos) {
  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5WellTreeView::onAddContainer);
}

void qColadaH5WellTreeView::hdrMenuRequested(QPoint pos) {
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

void qColadaH5WellTreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open well container"), "",
      QObject::tr("hdf5 file (*.h5 *.hdf5); all (*.*)"));

  addContainer(h5FileNameList);
}