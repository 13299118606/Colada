// Colada includes
#include "qColadaH5MapTreeView.h"
#include "qColadaH5ItemDelegate.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5MapModel.h"
#include "qColadaH5MapTreeView_p.h"

// Qt includes
#include <QAction>
#include <QHeaderView>
#include <QMenu>

// CTK includes
#include <ctkFileDialog.h>

// PythonQt includes
#include <PythonQt.h>

qColadaH5MapTreeViewPrivate::qColadaH5MapTreeViewPrivate(
    qColadaH5MapTreeView &q)
    : Superclass(q) {}

qColadaH5MapTreeViewPrivate::~qColadaH5MapTreeViewPrivate() {}

void qColadaH5MapTreeViewPrivate::init() {
  Q_Q(qColadaH5MapTreeView);
  this->Superclass::init();

  QSortFilterProxyModel* oldProxy =
      qobject_cast<QSortFilterProxyModel*>(q->model());
  if (oldProxy){
    delete oldProxy->sourceModel();
    delete oldProxy;
  }

  qColadaH5ProxyModel* proxy = new qColadaH5ProxyModel(q);
  qColadaH5MapModel* sm = new qColadaH5MapModel("Map data", q);
  proxy->setSourceModel(sm);
  q->setModel(proxy);
}

qColadaH5MapTreeView::qColadaH5MapTreeView(QWidget *parent)
    : qColadaH5TreeView(new qColadaH5MapTreeViewPrivate(*this), parent) {
  Q_D(qColadaH5MapTreeView);
  d->init();
}

qColadaH5MapTreeView::qColadaH5MapTreeView(
    qColadaH5MapTreeViewPrivate *pimpl, QWidget *parent)
    : qColadaH5TreeView(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5MapTreeView::~qColadaH5MapTreeView() {}

void qColadaH5MapTreeView::fillHdrMenu(QMenu *menu, QPoint pos) {
  this->Superclass::fillHdrMenu(menu, pos);

  QAction *readMapAct = menu->addAction("Read Map");
  connect(readMapAct, &QAction::triggered, this,
          &qColadaH5MapTreeView::onReadMap);
}

void qColadaH5MapTreeView::hdrMenuRequested(QPoint pos) {
  QMenu *menu = new QMenu(header());

  /* for subclasses to add actions */
  fillHdrMenu(menu, pos);

  // menu->popup(header()->mapToGlobal(pos));
  menu->exec(header()->mapToGlobal(pos));
  menu->deleteLater();
}

void qColadaH5MapTreeView::onReadMap() {
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();
  PythonQtObjectPtr context =
      mainModule.evalScript(QString("reader = colada.qColadaMapReader();"
                                    "reader.show();"));
}
