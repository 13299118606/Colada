// Colada includes
#include "qColadaH5SeisTreeView.h"
#include "qColadaH5SeisTreeView_p.h"
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5SeisModel.h"
#include "qColadaSegyReader.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>

// CTK includes
#include <ctkFileDialog.h>

qColadaH5SeisTreeViewPrivate::qColadaH5SeisTreeViewPrivate(
    qColadaH5SeisTreeView &q)
    : Superclass(q) {}

qColadaH5SeisTreeViewPrivate::~qColadaH5SeisTreeViewPrivate() {}

void qColadaH5SeisTreeViewPrivate::init() {
  Q_Q(qColadaH5SeisTreeView);
  this->Superclass::init();

  QSortFilterProxyModel* oldProxy =
      qobject_cast<QSortFilterProxyModel*>(q->model());
  if (oldProxy){
    delete oldProxy->sourceModel();
    delete oldProxy;
  }

  qColadaH5ProxyModel* proxy = new qColadaH5ProxyModel(q);
  qColadaH5SeisModel* sm = new qColadaH5SeisModel("Seismic data", q);
  proxy->setSourceModel(sm);
  q->setModel(proxy);
}

qColadaH5SeisTreeView::qColadaH5SeisTreeView(QWidget *parent)
    : qColadaH5TreeView(new qColadaH5SeisTreeViewPrivate(*this), parent) {
  Q_D(qColadaH5SeisTreeView);
  d->init();
}

qColadaH5SeisTreeView::qColadaH5SeisTreeView(
    qColadaH5SeisTreeViewPrivate *pimpl, QWidget *parent)
    : qColadaH5TreeView(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5SeisTreeView::~qColadaH5SeisTreeView() {}

void qColadaH5SeisTreeView::fillHdrMenu(QMenu *menu, QPoint pos) {
  this->Superclass::fillHdrMenu(menu, pos);

  QAction *readSegyAct = menu->addAction("Read SEGY");
  connect(readSegyAct, &QAction::triggered, this,
          &qColadaH5SeisTreeView::onReadSegy);
}

void qColadaH5SeisTreeView::hdrMenuRequested(QPoint pos) {
  QMenu *menu = new QMenu(header());

  /* for subclasses to add actions */
  fillHdrMenu(menu, pos);

  // menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5SeisTreeView::onReadSegy() {
  qColadaSegyReader *reader = new qColadaSegyReader();
  reader->show();
}
