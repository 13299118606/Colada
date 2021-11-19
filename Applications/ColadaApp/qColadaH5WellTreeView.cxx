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

// PythonQt includes
#include <PythonQt.h>

qColadaH5WellTreeViewPrivate::qColadaH5WellTreeViewPrivate(
    qColadaH5WellTreeView &q)
    : Superclass(q) {}

qColadaH5WellTreeViewPrivate::~qColadaH5WellTreeViewPrivate() {}

void qColadaH5WellTreeViewPrivate::init() {
  Q_Q(qColadaH5WellTreeView);
  this->Superclass::init();

  QSortFilterProxyModel* oldProxy =
      qobject_cast<QSortFilterProxyModel*>(q->model());
  if (oldProxy){
    delete oldProxy->sourceModel();
    delete oldProxy;
  }

  qColadaH5ProxyModel* proxy = new qColadaH5ProxyModel(q);
  qColadaH5WellModel* sm = new qColadaH5WellModel("Well data", q);
  proxy->setSourceModel(sm);
  q->setModel(proxy);
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
  this->Superclass::fillHdrMenu(menu, pos);

  QAction *readDevAct = menu->addAction("Read DEV");
  connect(readDevAct, &QAction::triggered, this,
          &qColadaH5WellTreeView::onReadDev);

  QAction *readLasAct = menu->addAction("Read LAS");
  connect(readLasAct, &QAction::triggered, this,
          &qColadaH5WellTreeView::onReadLas);
}

void qColadaH5WellTreeView::hdrMenuRequested(QPoint pos) {
  QMenu *menu = new QMenu(header());

  /* for subclasses to add actions */
  fillHdrMenu(menu, pos);

  // menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5WellTreeView::onReadLas() {
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();
  PythonQtObjectPtr context =
      mainModule.evalScript(QString("reader = colada.qColadaLasReader();"
                                    "reader.show();"));
}

void qColadaH5WellTreeView::onReadDev() {
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();
  PythonQtObjectPtr context =
      mainModule.evalScript(QString("reader = colada.qColadaDevReader();"
                                    "reader.show();"));
}
