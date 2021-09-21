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
  QAction *readSegyAct = menu->addAction("Read SEGY");
  connect(readSegyAct, &QAction::triggered, this,
          &qColadaH5SeisTreeView::onReadSegy);

  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5SeisTreeView::onAddContainer);
}

void qColadaH5SeisTreeView::hdrMenuRequested(QPoint pos) {
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

void qColadaH5SeisTreeView::onReadSegy() {
  qColadaSegyReader *reader = new qColadaSegyReader();
  reader->show();
}

void qColadaH5SeisTreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open seismic container"), "",
      QObject::tr("hdf5 file (*.h5 *.hdf5); all (*.*)"));

  addContainer(h5FileNameList);
}