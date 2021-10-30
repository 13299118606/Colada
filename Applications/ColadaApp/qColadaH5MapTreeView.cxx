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
  QAction *readMapAct = menu->addAction("Read Map");
  connect(readMapAct, &QAction::triggered, this,
          &qColadaH5MapTreeView::onReadMap);

  QAction *addContainerAction = menu->addAction("Add container");
  connect(addContainerAction, &QAction::triggered, this,
          &qColadaH5MapTreeView::onAddContainer);
}

void qColadaH5MapTreeView::hdrMenuRequested(QPoint pos) {
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

void qColadaH5MapTreeView::onReadMap() {
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();
  PythonQtObjectPtr context =
      mainModule.evalScript(QString("reader = colada.qColadaMapReader();"
                                    "reader.show();"));
}

void qColadaH5MapTreeView::onAddContainer() {
  QStringList h5FileNameList = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Open map container"), "",
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