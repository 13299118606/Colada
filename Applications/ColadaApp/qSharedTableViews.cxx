// Colada includes
#include "qSharedTableViews.h"
#include "qSharedTableViews_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>
#include <QTableView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QToolButton>
#include <QApplication>
#include <QHeaderView>
#include <QDebug>

// h5geo includes
#include <h5geo/h5util.h>

// magic_enum includes
#include <magic_enum.hpp>

qSharedTableViewsPrivate::qSharedTableViewsPrivate(qSharedTableViews &q)
    : q_ptr(&q) {}

qSharedTableViewsPrivate::~qSharedTableViewsPrivate() {}

void qSharedTableViewsPrivate::init() {
  Q_Q(qSharedTableViews);

  gridLayout = new QGridLayout(q);
  firstTableView = new QTableView(q);
  secondTableView = new QTableView(q);
  firstTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  secondTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  firstTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  secondTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  firstTableView->verticalHeader()->setDefaultSectionSize(
        firstTableView->verticalHeader()->minimumSectionSize());
  secondTableView->verticalHeader()->setDefaultSectionSize(
        secondTableView->verticalHeader()->minimumSectionSize());
  firstTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  firstTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  secondTableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  secondTableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  addToolBtn = new QToolButton(q);
  removeToolBtn = new QToolButton(q);
  QStandardItemModel* firstModel = new QStandardItemModel(firstTableView);
  QStandardItemModel* secondModel = new QStandardItemModel(secondTableView);
  firstTableView->setModel(firstModel);
  secondTableView->setModel(secondModel);

  // adds widgets to grid layout and sets icons for tool buttons
  q->setOrientation(Qt::Horizontal);

  q->connect(addToolBtn, &QAbstractButton::clicked,
             q, &qSharedTableViews::onAddToolBtnClicked);
  q->connect(removeToolBtn, &QAbstractButton::clicked,
             q, &qSharedTableViews::onRemoveToolBtnClicked);
  q->connect(firstTableView, &QAbstractItemView::doubleClicked,
             q, &qSharedTableViews::onFirstTableViewDoubleClicked);
  q->connect(secondTableView, &QAbstractItemView::doubleClicked,
             q, &qSharedTableViews::onSecondTableViewDoubleClicked);
}

qSharedTableViews::qSharedTableViews(QWidget *parent)
  : QWidget(parent), d_ptr(new qSharedTableViewsPrivate(*this)) {
  Q_D(qSharedTableViews);
  d->init();
}

qSharedTableViews::qSharedTableViews(qSharedTableViewsPrivate *pimpl, QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qSharedTableViews::~qSharedTableViews() {

}

void qSharedTableViews::setOrientation(Qt::Orientation orientation) {
  Q_D(qSharedTableViews);
  if (orientation == Qt::Vertical){
    d->gridLayout->addWidget(d->firstTableView, 0, 0, 2, 2);
    d->gridLayout->addWidget(d->addToolBtn, 2, 0, Qt::AlignmentFlag::AlignRight);
    d->gridLayout->addWidget(d->removeToolBtn, 2, 1, Qt::AlignmentFlag::AlignLeft);
    d->gridLayout->addWidget(d->secondTableView, 3, 0, 2, 2);
    d->addToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
    d->removeToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
  } else if (orientation == Qt::Horizontal){
    d->gridLayout->addWidget(d->firstTableView, 0, 0, 2, 2);
    d->gridLayout->addWidget(d->addToolBtn, 0, 2, Qt::AlignmentFlag::AlignBottom);
    d->gridLayout->addWidget(d->removeToolBtn, 1, 2, Qt::AlignmentFlag::AlignTop);
    d->gridLayout->addWidget(d->secondTableView, 0, 3, 2, 2);
    d->addToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    d->removeToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
  }
}

void qSharedTableViews::onAddToolBtnClicked(){
  Q_D(qSharedTableViews);
  this->onFirstTableViewDoubleClicked(d->firstTableView->currentIndex());
}

void qSharedTableViews::onRemoveToolBtnClicked(){
  Q_D(qSharedTableViews);
  this->onSecondTableViewDoubleClicked(d->secondTableView->currentIndex());
}

void qSharedTableViews::onFirstTableViewDoubleClicked(const QModelIndex &index)
{
  Q_D(qSharedTableViews);
  if (!index.isValid())
    return;

  auto firstModel = qobject_cast<QStandardItemModel*>(d->firstTableView->model());
  if (!firstModel){
    qDebug() << Q_FUNC_INFO << "Unable to get first source model";
    return;
  }

  auto secondModel = qobject_cast<QStandardItemModel*>(d->secondTableView->model());
  if (!secondModel){
    qDebug() << Q_FUNC_INFO << "Unable to get second source model";
    return;
  }

  QList<QStandardItem*> items;
  int row = index.row();
  for (int col = 0; col < firstModel->columnCount(); col++){
    items.append(firstModel->itemFromIndex(
                   firstModel->index(row, col))->clone());
  }

  secondModel->appendRow(items);
  firstModel->removeRow(d->firstTableView->currentIndex().row());
}

void qSharedTableViews::onSecondTableViewDoubleClicked(const QModelIndex &index)
{
  Q_D(qSharedTableViews);
  if (!index.isValid())
    return;

  auto firstModel = qobject_cast<QStandardItemModel*>(d->firstTableView->model());
  if (!firstModel){
    qDebug() << Q_FUNC_INFO << "Unable to get first source model";
    return;
  }

  auto secondModel = qobject_cast<QStandardItemModel*>(d->secondTableView->model());
  if (!secondModel){
    qDebug() << Q_FUNC_INFO << "Unable to get second source model";
    return;
  }

  QList<QStandardItem*> items;
  int row = index.row();
  for (int col = 0; col < secondModel->columnCount(); col++){
    items.append(secondModel->itemFromIndex(
                   secondModel->index(row, col))->clone());
  }

  firstModel->appendRow(items);
  secondModel->removeRow(d->secondTableView->currentIndex().row());
}

QTableView* qSharedTableViews::getFirstTableView(){
  Q_D(qSharedTableViews);
  return d->firstTableView;
}

QTableView* qSharedTableViews::getSecondTableView(){
  Q_D(qSharedTableViews);
  return d->secondTableView;
}
