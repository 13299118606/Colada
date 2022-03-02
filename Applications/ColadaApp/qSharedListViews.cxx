// Colada includes
#include "qSharedListViews.h"
#include "qSharedListViews_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QToolButton>
#include <QApplication>
#include <QDebug>

// h5geo includes
#include <h5geo/h5util.h>

// magic_enum includes
#include <magic_enum.hpp>

qSharedListViewsPrivate::qSharedListViewsPrivate(qSharedListViews &q)
    : q_ptr(&q) {}

qSharedListViewsPrivate::~qSharedListViewsPrivate() {}

void qSharedListViewsPrivate::init() {
  Q_Q(qSharedListViews);

  gridLayout = new QGridLayout(q);
  firstListView = new QListView(q);
  secondListView = new QListView(q);
  firstListView->setSelectionMode(QAbstractItemView::SingleSelection);
  secondListView->setSelectionMode(QAbstractItemView::SingleSelection);
  addToolBtn = new QToolButton(q);
  removeToolBtn = new QToolButton(q);
  QStandardItemModel* firstModel = new QStandardItemModel(firstListView);
  QStandardItemModel* secondModel = new QStandardItemModel(secondListView);
  firstListView->setModel(firstModel);
  secondListView->setModel(secondModel);

  // adds widgets to grid layout and sets icons for tool buttons
  q->setOrientation(Qt::Horizontal);

  q->connect(addToolBtn, &QAbstractButton::clicked,
             q, &qSharedListViews::onAddToolBtnClicked);
  q->connect(removeToolBtn, &QAbstractButton::clicked,
             q, &qSharedListViews::onRemoveToolBtnClicked);
  q->connect(firstListView, &QAbstractItemView::doubleClicked,
             q, &qSharedListViews::onFirstListViewDoubleClicked);
  q->connect(secondListView, &QAbstractItemView::doubleClicked,
             q, &qSharedListViews::onSecondListViewDoubleClicked);
}

qSharedListViews::qSharedListViews(QWidget *parent)
  : QWidget(parent), d_ptr(new qSharedListViewsPrivate(*this)) {
  Q_D(qSharedListViews);
  d->init();
}

qSharedListViews::qSharedListViews(qSharedListViewsPrivate *pimpl, QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qSharedListViews::~qSharedListViews() {

}

void qSharedListViews::setOrientation(Qt::Orientation orientation) {
  Q_D(qSharedListViews);
  if (orientation == Qt::Vertical){
    d->gridLayout->addWidget(d->firstListView, 0, 0, 2, 2);
    d->gridLayout->addWidget(d->addToolBtn, 2, 0, Qt::AlignmentFlag::AlignRight);
    d->gridLayout->addWidget(d->removeToolBtn, 2, 1, Qt::AlignmentFlag::AlignLeft);
    d->gridLayout->addWidget(d->secondListView, 3, 0, 2, 2);
    d->addToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
    d->removeToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
  } else if (orientation == Qt::Horizontal){
    d->gridLayout->addWidget(d->firstListView, 0, 0, 2, 2);
    d->gridLayout->addWidget(d->addToolBtn, 0, 2, Qt::AlignmentFlag::AlignBottom);
    d->gridLayout->addWidget(d->removeToolBtn, 1, 2, Qt::AlignmentFlag::AlignTop);
    d->gridLayout->addWidget(d->secondListView, 0, 3, 2, 2);
    d->addToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    d->removeToolBtn->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
  }
}

void qSharedListViews::onAddToolBtnClicked(){
  Q_D(qSharedListViews);
  this->onFirstListViewDoubleClicked(d->firstListView->currentIndex());
}

void qSharedListViews::onRemoveToolBtnClicked(){
  Q_D(qSharedListViews);
  this->onSecondListViewDoubleClicked(d->secondListView->currentIndex());
}

void qSharedListViews::onFirstListViewDoubleClicked(const QModelIndex &index)
{
  Q_D(qSharedListViews);
  if (!index.isValid())
    return;

  auto firstModel = qobject_cast<QStandardItemModel*>(d->firstListView->model());
  if (!firstModel){
    qDebug() << Q_FUNC_INFO << "Unable to get first source model";
    return;
  }

  auto secondModel = qobject_cast<QStandardItemModel*>(d->secondListView->model());
  if (!secondModel){
    qDebug() << Q_FUNC_INFO << "Unable to get second source model";
    return;
  }

  QStandardItem *item  = firstModel->itemFromIndex(
        d->firstListView->currentIndex())->clone();

  secondModel->appendRow(item);
  secondModel->sort(0, Qt::AscendingOrder);
  firstModel->removeRow(d->firstListView->currentIndex().row());
}

void qSharedListViews::onSecondListViewDoubleClicked(const QModelIndex &index)
{
  Q_D(qSharedListViews);
  if (!index.isValid())
    return;

  auto firstModel = qobject_cast<QStandardItemModel*>(d->firstListView->model());
  if (!firstModel){
    qDebug() << Q_FUNC_INFO << "Unable to get first source model";
    return;
  }

  auto secondModel = qobject_cast<QStandardItemModel*>(d->secondListView->model());
  if (!secondModel){
    qDebug() << Q_FUNC_INFO << "Unable to get second source model";
    return;
  }

  QStandardItem *item  = secondModel->itemFromIndex(
        d->secondListView->currentIndex())->clone();

  firstModel->appendRow(item);
  firstModel->sort(0, Qt::AscendingOrder);
  secondModel->removeRow(d->secondListView->currentIndex().row());
}

QListView* qSharedListViews::getFirstListView(){
  Q_D(qSharedListViews);
  return d->firstListView;
}

QListView* qSharedListViews::getSecondListView(){
  Q_D(qSharedListViews);
  return d->secondListView;
}
