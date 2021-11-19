// Colada includes
#include "qCopyPasteTableView.h"
#include "qCopyPasteTableView_p.h"

// Qt includes
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QThread>
#include <QHeaderView>

qCopyPasteTableViewPrivate::qCopyPasteTableViewPrivate(qCopyPasteTableView &q)
    : q_ptr(&q) {}

qCopyPasteTableViewPrivate::~qCopyPasteTableViewPrivate() {}

void qCopyPasteTableViewPrivate::init() {
  Q_Q(qCopyPasteTableView);
  // set row height to match content
  q->verticalHeader()->setDefaultSectionSize(q->verticalHeader()->minimumSectionSize());
  q->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  q->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

qCopyPasteTableView::qCopyPasteTableView(QWidget *parent) 
  : QTableView(parent), d_ptr(new qCopyPasteTableViewPrivate(*this)) {
  Q_D(qCopyPasteTableView);
  d->init();
}

qCopyPasteTableView::qCopyPasteTableView(qCopyPasteTableViewPrivate *pimpl, QWidget *parent)
    : QTableView(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qCopyPasteTableView::~qCopyPasteTableView() {

}

/* edit() works only on selected index !
 * thus setCurrentIndex(index); and edit(index);
 * updates the cell */
void qCopyPasteTableView::updateCellData(const QModelIndex &index) {
  setCurrentIndex(index);
  edit(index);
}

void qCopyPasteTableView::doCut() {
  doCopy();
  doDelete();
}

void qCopyPasteTableView::doCopy() {
  QString text;
  QItemSelectionRange range = selectionModel()->selection().first();
  for (int i = range.top(); i <= range.bottom(); ++i) {
    QStringList rowContents;
    for (int j = range.left(); j <= range.right(); ++j)
      rowContents << model()->index(i, j).data().toString();
    text += rowContents.join("\t");
    text += "\n";
  }
  QApplication::clipboard()->setText(text);
}

void qCopyPasteTableView::doPaste() {
  QString text = QApplication::clipboard()->text();
  QStringList rowContents = text.split("\n", QString::SkipEmptyParts);

  if (rowContents.count() == 1 &&
      rowContents[0].split("\t").count() == 1) { // if only one cell is pasted
    for (QModelIndex &index : selectedIndexes()) {
      if (model()->flags(index).testFlag(Qt::ItemIsEditable)) {
        model()->setData(index, rowContents[0]);
        updateCellData(index);
      }
    }
  } else { // if many cells is pasted
    QModelIndex initIndex = selectedIndexes().at(0);
    int initRow = initIndex.row();
    int initCol = initIndex.column();

    for (int i = 0; i < rowContents.size(); ++i) {
      QStringList columnContents = rowContents.at(i).split("\t");
      for (int j = 0; j < columnContents.size(); ++j) {
        QModelIndex index = model()->index(initRow + i, initCol + j);
        if (model()->flags(index).testFlag(Qt::ItemIsEditable)) {
          model()->setData(model()->index(initRow + i, initCol + j),
                           columnContents.at(j));
          updateCellData(index);
        }
      }
    }
  }
}

void qCopyPasteTableView::doDelete() {
  for (QModelIndex &index : selectedIndexes())
    if (model()->flags(index).testFlag(Qt::ItemIsEditable))
      model()->setData(index, QString());
}

void qCopyPasteTableView::keyPressEvent(QKeyEvent *event) {
  // at least one cell selected
  if (!selectedIndexes().isEmpty()) {
    if (event->key() == Qt::Key_Delete) {
      doDelete();
    } else if (event->matches(QKeySequence::Copy)) {
      doCopy();
    } else if (event->matches(QKeySequence::Paste)) {
      doPaste();
    } else if (event->matches(QKeySequence::Cut)) {
      doCut();
    } else
      QTableView::keyPressEvent(event);
  }
}
