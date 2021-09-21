// Colada includes
#include "qColadaReader.h"
#include "qColadaReader_p.h"

// Qt includes
#include <QLayout>

qColadaReaderPrivate::qColadaReaderPrivate(qColadaReader &q) : q_ptr(&q) {}

qColadaReaderPrivate::~qColadaReaderPrivate() {}

void qColadaReaderPrivate::init() {
  Q_Q(qColadaReader);
  q->setWindowFlags(
      q->windowFlags() &
      ~Qt::WindowContextHelpButtonHint & // removes question '?' sign
      Qt::WindowMaximizeButtonHint);     // add fullscreen button
  this->setupUi(q);

  q->connect(addToolBtn, &QToolButton::clicked, q,
             &qColadaReader::onAddBtnClicked);
  q->connect(removeToolBtn, &QToolButton::clicked, q,
             &qColadaReader::onRemoveToolBtnClicked);
  q->connect(autoDefineBtn, &QToolButton::clicked, q,
             &qColadaReader::onAutoDefineToolBtnClicked);
  q->connect(buttonBox, &QDialogButtonBox::clicked, q,
             &qColadaReader::onButtonBoxClicked);
  q->connect(model, &QStandardItemModel::dataChanged, q,
             &qColadaReader::onDataChanged);
}

void qColadaReaderPrivate::initVars(){};
void qColadaReaderPrivate::initTable(){};

void qColadaReaderPrivate::setupUi(QDialog *q) {
  tableView = new qCRSDropTableView();
  tableView->setSelectionMode(QTableView::ExtendedSelection);
  tableView->setAlternatingRowColors(true);
  tableView->setAcceptDrops(true);
  hrzHeader = new ctkCheckableHeaderView(Qt::Horizontal, tableView);
  hrzHeader->setSectionsClickable(true);
  hrzHeader->setSectionsMovable(false);
  hrzHeader->setHighlightSections(true);
  //    hrzHeader->setSectionResizeMode(QHeaderView::Stretch); // equally
  //    resizes column width
  tableView->setHorizontalHeader(hrzHeader);
  vertHeader = new ctkCheckableHeaderView(Qt::Vertical, tableView);
  vertHeader->setSectionsClickable(true);
  vertHeader->setSectionsMovable(false);
  vertHeader->setHighlightSections(true);
  tableView->setVerticalHeader(vertHeader);
  tableView->setSortingEnabled(true);
  proxy = new QSortFilterProxyModel();
  model = new QStandardItemModel();
  proxy->setSourceModel(model);
  tableView->setModel(proxy);
  addToolBtn = new QToolButton();
  addToolBtn->setToolTip("Add file");
  removeToolBtn = new QToolButton();
  removeToolBtn->setToolTip("Remove selected file");
  autoDefineBtn = new QToolButton();
  autoDefineBtn->setToolTip("Auto define");
  crsWidget = new qCRSWidget();
  crsWidget->getTableView()->setDragEnabled(true);
  buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  QWidget *widget = new QWidget();
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
  vSplitter->setOpaqueResize(false);
  vSplitter->addWidget(widget);
  vSplitter->addWidget(crsWidget);

  mainSplitter = new QSplitter(Qt::Horizontal);
  mainSplitter->setOpaqueResize(false);
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

void qColadaReader::onRemoveToolBtnClicked() {
  Q_D(qColadaReader);
  QModelIndexList indexList = d->tableView->selectionModel()->selectedRows();
  QList<int> rowList;

  for (QModelIndex &index : indexList)
    rowList.push_back(index.row());

  /* to remove rows correctly we should remove them in descending order */
  std::sort(rowList.rbegin(), rowList.rend()); // descending order sort

  for (int &row : rowList)
    d->model->removeRow(row);

  for (int row = 0; row < d->tableView->model()->rowCount(); row++)
    d->model->verticalHeaderItem(row)->setText(QString::number(row + 1));
}

void qColadaReader::onAutoDefineToolBtnClicked() {
  Q_D(qColadaReader);
  QModelIndexList indexList = d->tableView->selectionModel()->selectedRows();
  for (QModelIndex &index : indexList)
    updateRow(index.row());
}

void qColadaReader::onButtonBoxClicked(QAbstractButton *button) {}

void qColadaReader::onDataChanged(const QModelIndex &topLeft,
                                  const QModelIndex &bottomRight,
                                  const QVector<int> &roles) {}
