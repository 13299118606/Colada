// Colada includes
#include "qColadaReader.h"
#include "qColadaReader_p.h"
#include "qCRSDropTableView.h"
#include "qCRSWidget.h"

// Slicer includes
#include "qSlicerApplication.h"

// Qt includes
#include <QDebug>
#include <QLayout>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QToolButton>
#include <QSplitter>
#include <QDialogButtonBox>

// CTK includes
#include <ctkCheckableHeaderView.h>

qColadaReaderPrivate::qColadaReaderPrivate(qColadaReader &q) : q_ptr(&q) {}

qColadaReaderPrivate::~qColadaReaderPrivate() {}

void qColadaReaderPrivate::init() {
  Q_Q(qColadaReader);
  q->setWindowFlags(
      q->windowFlags() &
      ~Qt::WindowContextHelpButtonHint & // removes question '?' sign
      Qt::WindowMaximizeButtonHint);     // add fullscreen button
  this->setupUi(q);

  QObject::connect(addToolBtn, &QToolButton::clicked, q,
             &qColadaReader::onAddBtnClicked);
  QObject::connect(removeToolBtn, &QToolButton::clicked, q,
             &qColadaReader::onRemoveToolBtnClicked);
  QObject::connect(autoDefineBtn, &QToolButton::clicked, q,
             &qColadaReader::onAutoDefineToolBtnClicked);
  QObject::connect(buttonBox, &QDialogButtonBox::clicked, q,
             &qColadaReader::onButtonBoxClicked);
}

void qColadaReaderPrivate::initVars(){};
void qColadaReaderPrivate::initTable(){};

void qColadaReaderPrivate::setupUi(QDialog *q) {
  app = qSlicerApplication::application();
  if (!app){
    qCritical() << Q_FUNC_INFO << ": Unable to get application instance";
  }

  tableView = new qCRSDropTableView();
  tableView->setObjectName("TableView");
  tableView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  tableView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  tableView->setSelectionMode(QTableView::ExtendedSelection);
  tableView->setAlternatingRowColors(true);
  tableView->setAcceptDrops(true);
  hrzHeader = new ctkCheckableHeaderView(Qt::Horizontal, tableView);
  hrzHeader->setObjectName("HrzHeader");
  hrzHeader->setSectionsClickable(true);
  hrzHeader->setSectionsMovable(false);
  hrzHeader->setHighlightSections(true);
  //    hrzHeader->setSectionResizeMode(QHeaderView::Stretch); // equally
  //    resizes column width
  tableView->setHorizontalHeader(hrzHeader);
  vertHeader = new ctkCheckableHeaderView(Qt::Vertical, tableView);
  vertHeader->setObjectName("VertHeader");
  vertHeader->setSectionsClickable(true);
  vertHeader->setSectionsMovable(false);
  vertHeader->setHighlightSections(true);
  // set row height to match content
  vertHeader->setDefaultSectionSize(vertHeader->minimumSectionSize());
  tableView->setVerticalHeader(vertHeader);
  tableView->setSortingEnabled(true);
  proxy = new QSortFilterProxyModel();
  model = new QStandardItemModel();
  proxy->setSourceModel(model);
  tableView->setModel(proxy);
  addToolBtn = new QToolButton();
  addToolBtn->setObjectName("AddToolBtn");
  addToolBtn->setToolTip("Add file");
  addToolBtn->setIcon(QIcon(":/Icons/Add.png"));
  removeToolBtn = new QToolButton();
  removeToolBtn->setObjectName("RemoveToolBtn");
  removeToolBtn->setToolTip("Remove selected file");
  removeToolBtn->setIcon(QIcon(":/Icons/Remove.png"));
  autoDefineBtn = new QToolButton();
  autoDefineBtn->setObjectName("AutoDefineBtn");
  autoDefineBtn->setToolTip("Auto define");
  autoDefineBtn->setIcon(QIcon(":/Icons/Reload.png"));
  crsWidget = new qCRSWidget();
  crsWidget->setObjectName("CrsWidget");
  crsWidget->getTableView()->setDragEnabled(true);
  crsWidget->getUnitsLineEdit()->hide();
  buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->setObjectName("ButtonBox");
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  QWidget *widget = new QWidget();
  widget->setObjectName("TableWidget");
  QVBoxLayout *vLayout = new QVBoxLayout(widget);
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->addWidget(tableView);

  QHBoxLayout *hLayoutAddRemove = new QHBoxLayout();
  hLayoutAddRemove->setContentsMargins(0, 0, 0, 0);
  hLayoutAddRemove->addWidget(addToolBtn);
  hLayoutAddRemove->addWidget(removeToolBtn);
  hLayoutAddRemove->addWidget(autoDefineBtn);
  hLayoutAddRemove->insertStretch(0, 1);
  vLayout->addLayout(hLayoutAddRemove);

  vSplitter = new QSplitter(Qt::Vertical);
  vSplitter->setObjectName("VSplitter");
//  vSplitter->setOpaqueResize(false);
  vSplitter->addWidget(widget);
  vSplitter->addWidget(crsWidget);

  mainSplitter = new QSplitter(Qt::Horizontal);
  mainSplitter->setObjectName("MainSplitter");
//  mainSplitter->setOpaqueResize(false);
  mainSplitter->addWidget(vSplitter);

  QVBoxLayout *mainVLayout = new QVBoxLayout(q);
  mainVLayout->addWidget(mainSplitter);
  mainVLayout->addWidget(buttonBox);
}

qColadaReader::qColadaReader(QWidget *parent)
    : QDialog(parent), d_ptr(new qColadaReaderPrivate(*this)) {
  Q_D(qColadaReader);
  d->init();
}

qColadaReader::qColadaReader(qColadaReaderPrivate *pimpl, QWidget *parent)
    : QDialog(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qColadaReader::~qColadaReader() {}

void qColadaReader::updateRow(int row) {}

void qColadaReader::resetRow(int row) {}

void qColadaReader::onAddBtnClicked() {}

void qColadaReader::onRemoveToolBtnClicked() {}

void qColadaReader::onAutoDefineToolBtnClicked() {}

void qColadaReader::onButtonBoxClicked(QAbstractButton *button) {}
