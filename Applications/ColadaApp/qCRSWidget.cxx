//Colada includes
#include "qCRSWidget.h"
#include "qCRSWidget_p.h"
#include "dbcore.h"

// Qt insludes
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTextBrowser>
#include <QLineEdit>
#include <QSplitter>

// CTK includes
#include <ctkSearchBox.h>

// GDAL includes
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

// units includes
#include <units/units.hpp>

qCRSWidgetPrivate::qCRSWidgetPrivate(qCRSWidget& q)
	: q_ptr(&q)
{

}

qCRSWidgetPrivate::~qCRSWidgetPrivate() {

}

void qCRSWidgetPrivate::init() {
	Q_Q(qCRSWidget);
	this->setupUi(q);
	this->setupTableData(q);
}

void qCRSWidgetPrivate::setupUi(QWidget* q) {
	searchBox = new ctkSearchBox();
  unitsLineEdit = new QLineEdit();
  unitsLineEdit->setPlaceholderText("Units...");
  tableView = new QTableView();
  tableView->setSortingEnabled(true);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	proxy = new QSortFilterProxyModel(tableView);
	model = new QStandardItemModel(proxy);
	proxy->setSourceModel(model);
	tableView->setModel(proxy);
	txtBrowser = new QTextBrowser();
	txtBrowser->setReadOnly(true);
	txtBrowser->viewport()->setCursor(Qt::IBeamCursor); // set cursor type

	splitter = new QSplitter(Qt::Horizontal);
	splitter->setOpaqueResize(false);
	splitter->addWidget(tableView);
	splitter->addWidget(txtBrowser);

  mainLayout = new QVBoxLayout(q);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->addWidget(unitsLineEdit, 0, Qt::AlignLeft);
  mainLayout->addWidget(searchBox);
  mainLayout->addWidget(splitter, 1);
}

void qCRSWidgetPrivate::setupTableData(QWidget* q) {
	QStringList authNameList, codeList, nameList;
	if (!dbcore::getAvailableProjections(
		authNameList, codeList, nameList))
		return;

	model->setColumnCount(3);
	model->setHorizontalHeaderLabels({ "name",
																	 "auth name",
																	 "code" });

	model->horizontalHeaderItem(0)->setToolTip("CRS name");
	model->horizontalHeaderItem(1)->setToolTip("Authority name");
	model->horizontalHeaderItem(2)->setToolTip("Code");

	model->insertRows(0, authNameList.count());
	for (int i = 0; i < authNameList.count(); i++) {
		model->setData(model->index(i, 0), nameList[i]);
		model->setData(model->index(i, 1), authNameList[i]);
		model->setData(model->index(i, 2), codeList[i]);
	}
}

qCRSWidget::qCRSWidget(QWidget* parent) :
	QWidget(parent), d_ptr(new qCRSWidgetPrivate(*this))
{
	Q_D(qCRSWidget);
	d->init();

	QItemSelectionModel* selectionModel = d->tableView->selectionModel();

	connect(d->searchBox, &ctkSearchBox::editingFinished, this, &qCRSWidget::onSearchBoxEditingFinished);
	connect(selectionModel, &QItemSelectionModel::selectionChanged,
		this, &qCRSWidget::onModelSelectionChanged);
}

qCRSWidget::~qCRSWidget() {

}

void qCRSWidget::onSearchBoxEditingFinished()
{
	Q_D(qCRSWidget);

	QList<QStandardItem*> itemList = d->model->findItems(
		d->searchBox->text(), Qt::MatchContains);

	for (int i = 0; i < d->proxy->rowCount(); i++) {
		d->tableView->verticalHeader()->hideSection(i);
	}

	for (int i = 0; i < itemList.count(); i++) {
		int row = d->proxy->mapFromSource(d->model->index(itemList[i]->row(), 0)).row();
		d->tableView->verticalHeader()->showSection(row);
	}
}

void qCRSWidget::onModelSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected) {
  Q_D(qCRSWidget);

  if (selected.isEmpty())
    return;

  /* Table selectionMode is SingleSelection.
   * Selection behavior is QAbstractItemView::SelectRows */
  QModelIndex index = selected.indexes()[0];
  if (!index.isValid())
    return;

  QString name = d->proxy->data(d->proxy->index(index.row(), 0)).toString();
  QString authName = d->proxy->data(d->proxy->index(index.row(), 1)).toString();
  QString code = d->proxy->data(d->proxy->index(index.row(), 2)).toString();
  QString units = getUnits();

  double coef = units::convert(units::unit_from_string(units.toStdString()),
      units::unit_from_string("meter"));

  OGRSpatialReference sr;
  sr.SetFromUserInput((authName + ":" + code).toUtf8());
  sr.SetLinearUnitsAndUpdateParameters(units.toUtf8(), coef);

  char *pszWKT = nullptr;
  char *pszProj4 = nullptr;
  const char *apszOptions[] = {"FORMAT=WKT2_2018", "MULTILINE=YES", nullptr};
  sr.exportToWkt(&pszWKT, apszOptions);
  sr.exportToProj4(&pszProj4);

  QFont font;
  font.setBold(true);
  d->txtBrowser->setCurrentFont(font);
  d->txtBrowser->setText("WKT");
  font.setBold(false);
  d->txtBrowser->setCurrentFont(font);
  d->txtBrowser->append(pszWKT);
  d->txtBrowser->append(""); // skip line
  font.setBold(true);
  d->txtBrowser->setCurrentFont(font);
  d->txtBrowser->append("PROJ4");
  font.setBold(false);
  d->txtBrowser->setCurrentFont(font);
  d->txtBrowser->append(pszProj4);
  d->txtBrowser->viewport()->setCursor(Qt::IBeamCursor);
}

QString qCRSWidget::getUnits() {
  Q_D(qCRSWidget);
  QString units = d->unitsLineEdit->text();
  if (units.isEmpty())
    units = "meter";
  return units;
}

ctkSearchBox* qCRSWidget::getSearchBox() {
	Q_D(qCRSWidget);
	return d->searchBox;
}

QLineEdit* qCRSWidget::getUnitsLineEdit() {
  Q_D(qCRSWidget);
  return d->unitsLineEdit;
}

QTableView* qCRSWidget::getTableView() {
	Q_D(qCRSWidget);
	return d->tableView;
}

QTextBrowser* qCRSWidget::getTextBrowser() {
	Q_D(qCRSWidget);
	return d->txtBrowser;
}

QSortFilterProxyModel* qCRSWidget::getProxyModel() {
	Q_D(qCRSWidget);
	return d->proxy;
}

QStandardItemModel* qCRSWidget::getModel() {
	Q_D(qCRSWidget);
	return d->model;
}

QVBoxLayout *qCRSWidget::getMainLayout() {
  Q_D(qCRSWidget);
  return d->mainLayout;
}

QSplitter *qCRSWidget::getSplitter() {
  Q_D(qCRSWidget);
  return d->splitter;
}