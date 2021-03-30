// Colada includes
#include "qCRSWidget.h"

//// Qt insludes
//#include <QLayout>
//#include <QTableView>
//#include <QHeaderView>
//#include <QSortFilterProxyModel>
//#include <QStandardItemModel>
//#include <QTextBrowser>
//#include <QSplitter>
//#include <ctkSearchBox.h>
//#include <gdal.h>
//#include <gdal_priv.h>
//
//class qCRSWidgetPrivate
//{
//  Q_DECLARE_PUBLIC(qCRSWidget);
//
//protected:
//  qCRSWidget* const q_ptr;
//
//public:
//  qCRSWidgetPrivate(qCRSWidget& object);
//
//  void init();
//  void setupUi(QWidget* coladaNewProject);
//
//private:
//  void initTableData(); // I need to implement this
//
//public:
//  QTableView* tableView;
//  QSortFilterProxyModel* proxy;
//  QStandardItemModel* model;
//  ctkSearchBox* searchBox;
//  QTextBrowser* txtBrowser;
//};
//
//qCRSWidgetPrivate::qCRSWidgetPrivate(qCRSWidget& object)
//  : q_ptr(&object)
//{
//
//}
//
//void qCRSWidgetPrivate::init() {
//  Q_Q(qCRSWidget);
//  this->setupUi(q);
//}
//
//void qCRSWidgetPrivate::setupUi(QWidget* coladaNewProject) {
//  searchBox = new ctkSearchBox();
//  tableView = new QTableView();
//  tableView->setSortingEnabled(true);
//  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
//  tableView->setSelectionMode(QAbstractItemView::SingleSelection);
//  tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
//  proxy = new QSortFilterProxyModel(tableView);
//  model = new QStandardItemModel(proxy);
//  proxy->setSourceModel(model);
//  tableView->setModel(proxy);
//  txtBrowser = new QTextBrowser();
//  txtBrowser->setReadOnly(true);
//  txtBrowser->viewport()->setCursor(Qt::IBeamCursor); // set cursor type
//
//  QSplitter* splitter = new QSplitter(Qt::Horizontal);
//  splitter->setOpaqueResize(false);
//  splitter->addWidget(tableView);
//  splitter->addWidget(txtBrowser);
//
//  QVBoxLayout* mainLayout = new QVBoxLayout(this);
//  mainLayout->setContentsMargins(0, 0, 0, 0);
//  mainLayout->addWidget(searchBox, 0);
//  mainLayout->addWidget(splitter, 1);
//}