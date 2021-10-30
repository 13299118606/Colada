// Colada includes
#include "qColadaSegyReader.h"
#include "qColadaSegyReader_p.h"
#include "qComboBoxDelegate.h"
#include "qSpinBoxDelegate.h"
#include "qScienceSpinBoxDelegate.h"
#include "qREValidator.h"
#include "qLineEditDelegate.h"
#include "qPathEditDelegate.h"
#include "dbcore.h"
#include "util.h"
#include "SegyRead.h"
#include "qCRSWidget.h"
#include "qCRSDropTableView.h"

// Qt includes
#include <QLayout>
#include <QMessageBox>
#include <QRadioButton>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QStandardItemModel>
#include <QPushButton>
#include <QSplitter>

// CTK includes
#include <ctkFileDialog.h>
#include <ctkCheckableHeaderView.h>

// h5geo includes
#include <h5geo/h5core.h>

// magic_enum includes
#include <magic_enum.hpp>

qColadaSegyReaderPrivate::qColadaSegyReaderPrivate(qColadaSegyReader &q)
    : Superclass(q) {}

qColadaSegyReaderPrivate::~qColadaSegyReaderPrivate() {}

void qColadaSegyReaderPrivate::init() {
  Q_Q(qColadaSegyReader);
  this->Superclass::init(); // must be before current class initialization
  initVars();
  initTable();
  initTxtHdrBrowser();
  initTrcHdrBytesTable();
}

void qColadaSegyReaderPrivate::initVars() {
  tableHdrNames = QStringList(
      {"plot", "read file", "save to", "CRS", "seis name", "chunk size",
       "N threads", "creation type", "survey type", "data type", "domain",
       "text encoding", "endianness", "format", "spatial units",
       "temporal units", "data units", "SRD"});

  tableHdrTips = QStringList(
      {"Plot data", "Read file", "Container where to save data",
       "CRS authority name and code (example: EPSG:2000)",
       "Seismic name (if empty then name will be autogenerated)",
       "Number of traces to chunk and to read at once (heap size)", 
       "Number of threads to read SEGY in parallel",
       "Creation type for seismic", "2D/3D",
       "STACK, PRESTACK",
       "May be time or depth", "Textual header encoding", "SEGY endianness",
       "SEGY format", "Spatial units", "Temporal units", "Data units (probably pressure: pascal, psi, atmosphere)",
       "Seismic Reference Datum"});

  std::vector<std::string> fullHeaderNameList, shortHeaderNameList;
  h5geo::getBinHeaderNames(fullHeaderNameList, shortHeaderNameList);

  for (size_t i = 0; i < fullHeaderNameList.size(); i++) {
    tableHdrNames.push_back(QString::fromStdString(shortHeaderNameList[i]));
    tableHdrTips.push_back(QString::fromStdString(fullHeaderNameList[i]));
  }

  spinboxColNameList = QStringList({"chunk size", "N threads"});
  spinboxColNameList.append(util::toQStringList(shortHeaderNameList));

  scSpinboxColNameList = QStringList({"SRD"});

  lineEditColNameList = QStringList({"seis name"});

  pathEditColNameList = QStringList({"save to"});
}

void qColadaSegyReaderPrivate::initTable() {
  model->setColumnCount(tableHdrNames.count());
  model->setHorizontalHeaderLabels(tableHdrNames);
  for (int i = 0; i < model->columnCount(); i++) {
    model->horizontalHeaderItem(i)->setToolTip(tableHdrTips[i]);
  }

  /* ComboBoxDelegate */
  util::setComboDelegateFromEnumClassForTable<h5geo::CreationType>(
      tableView, tableHdrNames.indexOf("creation type"));
  util::setComboDelegateFromEnumClassForTable<h5geo::SurveyType>(
      tableView, tableHdrNames.indexOf("survey type"));
  util::setComboDelegateFromEnumClassForTable<h5geo::SeisDataType>(
      tableView, tableHdrNames.indexOf("data type"));
  util::setComboDelegateFromEnumClassForTable<h5geo::Domain>(
      tableView, tableHdrNames.indexOf("domain"));
  util::setComboDelegateFromEnumClassForTable<h5geo::TxtEncoding>(
      tableView, tableHdrNames.indexOf("text encoding"));
  util::setComboDelegateFromEnumClassForTable<h5geo::SegyEndian>(
      tableView, tableHdrNames.indexOf("endianness"));
  util::setComboDelegateFromEnumClassForTable<h5geo::SegyFormat>(
      tableView, tableHdrNames.indexOf("format"));

  /* SpinBoxDelegate */
  qSpinBoxDelegate *spinBoxDelegate = new qSpinBoxDelegate(0);
  spinBoxDelegate->setParent(tableView);
  for (auto &name : spinboxColNameList) {
    int col = tableHdrNames.indexOf(name);
    tableView->setItemDelegateForColumn(col, spinBoxDelegate);
  }

  /* ScienceSpinBoxDelegate */
  qScienceSpinBoxDelegate *scienceSpinBoxDelegate =
      new qScienceSpinBoxDelegate();
  scienceSpinBoxDelegate->setParent(tableView);
  for (auto &name : scSpinboxColNameList) {
    int col = tableHdrNames.indexOf(name);
    tableView->setItemDelegateForColumn(col, scienceSpinBoxDelegate);
  }

  /* LineEditDelegate */
  QRegularExpression re = util::fileNameRegExp();
  qREValidator *validator =
      new qREValidator(re, util::fileNameToolTipText(), 3000, tableView);
  qLineEditDelegate *lineEditDelegate =
      new qLineEditDelegate(QString(), validator, tableView);
  for (auto &name : lineEditColNameList) {
    int col = tableHdrNames.indexOf(name);
    tableView->setItemDelegateForColumn(col, lineEditDelegate);
  }

  /* PathEditDelegate */
  qPathEditDelegate *pathDelegate = new qPathEditDelegate();
  pathDelegate->setParent(tableView);
  for (auto &name : pathEditColNameList) {
    int col = tableHdrNames.indexOf(name);
    tableView->setItemDelegateForColumn(col, pathDelegate);
  }
}

void qColadaSegyReaderPrivate::initTxtHdrBrowser() {
  txtHdrBrowser = new QTextBrowser();
  txtHdrBrowser->setLineWrapColumnOrWidth(80);
  txtHdrBrowser->setLineWrapMode(QTextEdit::FixedColumnWidth);
  txtHdrBrowser->setTabStopDistance(80);

  hSplitter = new QSplitter(Qt::Horizontal);
  hSplitter->setOpaqueResize(false);
  hSplitter->addWidget(crsWidget);
  hSplitter->addWidget(txtHdrBrowser);

  vSplitter->addWidget(hSplitter);
}

void qColadaSegyReaderPrivate::initTrcHdrBytesTable(){
  trcHdrBytesTableView = new qCopyPasteTableView();
  trcHdrBytesTableView->setObjectName("TrcHdrBytesTableView");
  trcHdrBytesTableView->setSelectionMode(QTableView::ExtendedSelection);
  trcHdrBytesTableView->setAlternatingRowColors(true);
  trcHdrBytesTableView->setAcceptDrops(true);
  trcHdrBytesTableView->setSortingEnabled(true);
  ctkCheckableHeaderView *hrzHeader = new ctkCheckableHeaderView(Qt::Horizontal, tableView);
  hrzHeader->setObjectName("HrzHeader");
  hrzHeader->setSectionsClickable(true);
  hrzHeader->setSectionsMovable(false);
  hrzHeader->setHighlightSections(true);
  //    hrzHeader->setSectionResizeMode(QHeaderView::Stretch); // equally
  //    resizes column width
  trcHdrBytesTableView->setHorizontalHeader(hrzHeader);
  ctkCheckableHeaderView *vertHeader = new ctkCheckableHeaderView(Qt::Vertical, tableView);
  vertHeader->setObjectName("VertHeader");
  vertHeader->setSectionsClickable(true);
  vertHeader->setSectionsMovable(false);
  vertHeader->setHighlightSections(true);
  trcHdrBytesTableView->setVerticalHeader(vertHeader);

  vSplitter->insertWidget(1, trcHdrBytesTableView);

  trcHdrBytesProxy = new QSortFilterProxyModel();
  trcHdrBytesModel = new QStandardItemModel();
  trcHdrBytesProxy->setSourceModel(trcHdrBytesModel);
  trcHdrBytesTableView->setModel(trcHdrBytesProxy);

  std::vector<int> trcHdrBytes, trcHdrNBytes;
  h5geo::getTraceHeaderBytes(trcHdrBytes, trcHdrNBytes);

  std::vector<std::string> trcHdrNames, trcHdrShortNames;
  h5geo::getTraceHeaderNames(trcHdrNames, trcHdrShortNames);

  QStringList trcHdrTableNames, trcHdrShortComboNames;
  trcHdrTableNames.push_back("read file");
  for(size_t i = 0; i < trcHdrBytes.size(); i++){
    trcHdrTableNames.push_back(
          QString::number(trcHdrBytes[i]) + ": " +
          QString::number(trcHdrNBytes[i]));
    trcHdrShortComboNames.push_back(QString::fromStdString(trcHdrShortNames[i]));
  }

  trcHdrBytesModel->setColumnCount(trcHdrBytes.size());
  trcHdrBytesModel->setHorizontalHeaderLabels(trcHdrTableNames);

  for (int col = 0; col < trcHdrBytesModel->columnCount(); col++) {
    if (col == 0)
      trcHdrBytesModel->horizontalHeaderItem(col)->setToolTip("Read file");
    else
      trcHdrBytesModel->horizontalHeaderItem(col)->setToolTip("Trace header bytes start and length");
  }

  for (int col = 1; col < trcHdrBytesModel->columnCount(); col++){
    qComboBoxDelegate* comboDelegate =
        new qComboBoxDelegate(trcHdrShortComboNames, trcHdrBytesTableView);
    trcHdrBytesTableView->setItemDelegateForColumn(col, comboDelegate);
  }
}

qColadaSegyReader::qColadaSegyReader(QWidget *parent)
    : qColadaReader(new qColadaSegyReaderPrivate(*this), parent) {
  Q_D(qColadaSegyReader);
  d->init();

  connect(d->trcHdrBytesModel, &QStandardItemModel::dataChanged,
          this, &qColadaSegyReader::onBytesTableDataChanged);
}

qColadaSegyReader::~qColadaSegyReader() {}

void qColadaSegyReader::updateRow(int proxy_row) {
  Q_D(qColadaSegyReader);

  QString readFile =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("read file")))
          .toString();
  QString errMsg;
  SegyRead segyRead(readFile, errMsg);
  if (!errMsg.isEmpty()) {
    QMessageBox::critical(this, "Error", errMsg);
    return;
  }

  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("ebcdic/ascii")),
                 segyRead.getTxtEncoding());

  QString endianStr = segyRead.getSegyEndian();
  auto endian =
      magic_enum::enum_cast<h5geo::SegyEndian>(endianStr.toStdString());
  if (!endian.has_value()) {
    return;
  }

  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("endianness")),
                 endianStr);
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("format")),
                    segyRead.getSegyFormat(endian.value()));
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("text encoding")),
      segyRead.getTxtEncoding());

  double binHdr[30];
  segyRead.readBinHdr(binHdr, endian.value());

  std::vector<std::string> fullHeaderNameList, shortHeaderNameList;
  h5geo::getBinHeaderNames(fullHeaderNameList, shortHeaderNameList);
  for (int i = 0; i < shortHeaderNameList.size(); i++) {
    d->proxy->setData(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf(QString::fromStdString(shortHeaderNameList[i]))),
        binHdr[i]);
  }
}

void qColadaSegyReader::resetRow(int proxy_row) {
  Q_D(qColadaSegyReader);

  QString readFile =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("read file")))
          .toString();
  QFileInfo fi(readFile);
  if (!fi.exists())
    return;

  QString seisName = fi.baseName();
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("chunk size")), 10000);
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("N threads")),
                    4);
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("seis name")),
                    seisName);
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("save to")),
                    dbcore::getSeisDir() + "/" + seisName + ".h5");
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("CRS")),
                    dbcore::getCurrentProjectionNameCode());
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("creation type")),
      QString::fromStdString(std::string{
          magic_enum::enum_name(h5geo::CreationType::OPEN_OR_CREATE)}));
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("survey type")), "TWO_D");
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("data type")),
                    "PRESTACK");
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("domain")),
                    "TWT");
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("spatial units")), "meter");
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("temporal units")), "ms");
  d->proxy->setData(
      d->proxy->index(proxy_row, d->tableHdrNames.indexOf("data units")), "psi");
  d->proxy->setData(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("SRD")), 0);
}

SegyRead::ReadOnlyParam
qColadaSegyReader::getReadOnlyParamFromTable(int proxy_row, QString &errMsg) {
  Q_D(qColadaSegyReader);

  SegyRead::ReadOnlyParam p;
  p.readFile =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("read file")))
          .toString();
  p.crs = d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("CRS")))
              .toString();

  std::string str;
  str = d->proxy
            ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("endianness")))
            .toString()
            .toStdString();
  auto endian_opt = magic_enum::enum_cast<h5geo::SegyEndian>(str);
  if (!endian_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `endianness`!";
    return p;
  }
  p.endian = endian_opt.value();

  str = d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("format")))
            .toString()
            .toStdString();
  auto format_opt = magic_enum::enum_cast<h5geo::SegyFormat>(str);
  if (!format_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `format`!";
    return p;
  }
  p.format = format_opt.value();

  p.spatialUnits = d->proxy
                       ->data(d->proxy->index(
                           proxy_row, d->tableHdrNames.indexOf("spatial units")))
                       .toString()
                       .toStdString();

  p.temporalUnits = d->proxy
                        ->data(d->proxy->index(
                            proxy_row, d->tableHdrNames.indexOf("temporal units")))
                        .toString()
                        .toStdString();

  p.dataUnits = d->proxy
                        ->data(d->proxy->index(
                            proxy_row, d->tableHdrNames.indexOf("data units")))
                        .toString()
                        .toStdString();

  std::vector<std::string> fullHeaderNameList, shortHeaderNameList;
  h5geo::getBinHeaderNames(fullHeaderNameList, shortHeaderNameList);
  for (int i = 0; i < shortHeaderNameList.size(); i++) {
    QVariant var = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf(QString::fromStdString(shortHeaderNameList[i]))));
    if (var.isNull())
      p.binHdr[i] = 0;
    else
      p.binHdr[i] = var.toDouble();
  }
  return p;
}

SegyRead::ReadWriteParam
qColadaSegyReader::getReadWriteParamFromTable(int proxy_row, QString &errMsg) {
  Q_D(qColadaSegyReader);

  QVariant var;
  SegyRead::ReadWriteParam p;
  p.seisName = 
    d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("seis name")))
          .toString();
  p.traceHeapSize =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("chunk size")))
          .toULongLong();
  p.nThread2use =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("N threads")))
          .toULongLong();
  p.readFile =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("read file")))
          .toString();
  p.saveFile =
      d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("save to")))
          .toString();
  p.crs = d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("CRS")))
              .toString();

  std::string str;
  str = d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("creation type")))
          .toString()
          .toStdString();
  auto seisCreateType_opt = magic_enum::enum_cast<h5geo::CreationType>(str);
  if (!seisCreateType_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `creation type`!";
    return p;
  }
  p.seisCreateType = seisCreateType_opt.value();

  str = d->proxy
            ->data(
                d->proxy->index(proxy_row, d->tableHdrNames.indexOf("survey type")))
            .toString()
            .toStdString();
  auto surveyType_opt = magic_enum::enum_cast<h5geo::SurveyType>(str);
  if (!surveyType_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `survey type`!";
    return p;
  }
  p.surveyType = surveyType_opt.value();

  str = d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("data type")))
          .toString()
          .toStdString();
  auto dataType_opt = magic_enum::enum_cast<h5geo::SeisDataType>(str);
  if (!dataType_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `data type`!";
    return p;
  }
  p.dataType = dataType_opt.value();

  str = d->proxy
            ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("domain")))
            .toString()
            .toStdString();
  auto domain_opt = magic_enum::enum_cast<h5geo::Domain>(str);
  if (!domain_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `domain`!";
    return p;
  }
  p.domain = domain_opt.value();

  str = d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("text encoding")))
            .toString()
            .toStdString();
  auto encoding_opt = magic_enum::enum_cast<h5geo::TxtEncoding>(str);
  if (!encoding_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `text encoding`!";
    return p;
  }
  p.encoding = encoding_opt.value();

  str = d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("endianness")))
          .toString()
          .toStdString();
  auto endian_opt = magic_enum::enum_cast<h5geo::SegyEndian>(str);
  if (!endian_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `endianness`!";
    return p;
  }
  p.endian = endian_opt.value();

  str = d->proxy
            ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("format")))
            .toString()
            .toStdString();
  auto format_opt = magic_enum::enum_cast<h5geo::SegyFormat>(str);
  if (!format_opt.has_value()) {
    errMsg = p.readFile + ": Inapropriate `format`!";
    return p;
  }
  p.format = format_opt.value();

  p.spatialUnits = d->proxy
                       ->data(d->proxy->index(
                           proxy_row, d->tableHdrNames.indexOf("spatial units")))
                       .toString()
                       .toStdString();

  p.temporalUnits = d->proxy
                        ->data(d->proxy->index(
                            proxy_row, d->tableHdrNames.indexOf("temporal units")))
                        .toString()
                        .toStdString();

  p.dataUnits =
      d->proxy
          ->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("data units")))
          .toString()
          .toStdString();

  var = d->proxy->data(d->proxy->index(proxy_row, d->tableHdrNames.indexOf("SRD")))
            .toString();
  if (var.isNull())
    p.srd = 0;
  else
    p.srd = var.toDouble();

  std::vector<std::string> fullHeaderNameList, shortHeaderNameList;
  h5geo::getBinHeaderNames(fullHeaderNameList, shortHeaderNameList);
  for (int i = 0; i < shortHeaderNameList.size(); i++) {
    var = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf(QString::fromStdString(shortHeaderNameList[i]))));
    if (var.isNull())
      p.binHdr[i] = 0;
    else
      p.binHdr[i] = var.toDouble();
  }

  SegyRead segyRead(p.readFile, errMsg);
  if (!errMsg.isEmpty()) {
    QMessageBox::critical(this, "Error", errMsg);
    return p;
  }
  segyRead.readTxtHdr(p.txtHdr, p.encoding);
  return p;
}

std::vector<std::string> qColadaSegyReader::getTrcHdrNamesFromBytesTable(
    const QString& readFile)
{
  Q_D(qColadaSegyReader);
  QSortFilterProxyModel* proxy = dynamic_cast<QSortFilterProxyModel*>(
        d->trcHdrBytesTableView->model());
  QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(
        proxy->sourceModel());

  QList<QStandardItem*> itemList = model->findItems(
          readFile, Qt::MatchFixedString, 0);

  if (itemList.isEmpty()){
    return std::vector<std::string>();
  }

  int row = itemList[0]->row();
  std::vector<std::string> hdrNames(model->columnCount()-1);
  for (int col = 1; col < model->columnCount(); col++)
    hdrNames[col-1] = model->data(
          model->index(row, col)).
        toString().toStdString();
  return hdrNames;
}

void qColadaSegyReader::addBytesTableRow(const QString& readFile){
  Q_D(qColadaSegyReader);
  QSortFilterProxyModel* proxy = dynamic_cast<QSortFilterProxyModel*>(
        d->trcHdrBytesTableView->model());
  QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(
        proxy->sourceModel());

  if (!model->findItems(
        readFile, Qt::MatchFixedString, 0).isEmpty()){
    return;
  }

  model->insertRow(model->rowCount());
  int row = model->rowCount() - 1;
  int proxy_row = proxy->mapFromSource(
        model->index(row, 0)).row();

  QStandardItem *readFileItem = new QStandardItem(readFile);
  readFileItem->setFlags(readFileItem->flags() & ~Qt::ItemIsEditable);
  model->setItem(row, 0, readFileItem);

  std::vector<std::string> trcHdrNames, trcHdrShortNames;
  h5geo::getTraceHeaderNames(trcHdrNames, trcHdrShortNames);

  model->blockSignals(true);
  proxy->blockSignals(true);
  for (int col = 1; col < proxy->columnCount(); col++){
    proxy->setData(
        proxy->index(proxy_row, col),
          QString::fromStdString(trcHdrShortNames[col-1]));
  }
  proxy->blockSignals(false);
  model->blockSignals(false);

  model->verticalHeaderItem(row)->setText(
      QString::number(model->rowCount()));
}

void qColadaSegyReader::removeBytesTableRow(const QString& readFile){
  Q_D(qColadaSegyReader);
  QSortFilterProxyModel* proxy = dynamic_cast<QSortFilterProxyModel*>(
        d->trcHdrBytesTableView->model());
  QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(
        proxy->sourceModel());

  QList<QStandardItem*> itemList = model->findItems(
          readFile, Qt::MatchFixedString, 0);

  if (itemList.isEmpty()){
    return;
  }

  QList<int> rowList;
  for (QStandardItem* item : itemList)
    rowList.push_back(item->row());

  /* to remove rows correctly we should remove them in descending order */
  std::sort(rowList.rbegin(), rowList.rend()); // descending order sort

  // as item are taken from `model` then item rows are referred to `model` (not `proxy`)
  for (int &row : rowList)
    model->removeRow(row);

  for (int row = 0; row < model->rowCount(); row++)
    model->verticalHeaderItem(row)->setText(QString::number(row + 1));
}

void qColadaSegyReader::onAddBtnClicked() {
  Q_D(qColadaSegyReader);
  QStringList fileNames = ctkFileDialog::getOpenFileNames(
      nullptr, QObject::tr("Select one or more SEGY files to open"), "",
      QObject::tr("SEGY (*.seg *.sgy *.segy);; all (*.*)"));

  for (auto name : fileNames) {
    if (!d->model
             ->findItems(name, Qt::MatchFixedString,
                         d->tableHdrNames.indexOf("read file"))
             .isEmpty()) {
      QMessageBox::warning(this, "Warning", name + ": is already in the table!");
      continue;
    }

    d->model->insertRow(d->model->rowCount());
    int row = d->model->rowCount() - 1;
    int proxy_row = d->proxy->mapFromSource(
      d->model->index(row, 0)).row();

    d->tableView->setIndexWidget(
        d->model->index(row, d->tableHdrNames.indexOf("plot")),
        new QRadioButton());

    QStandardItem *readFileItem = new QStandardItem(name);
    readFileItem->setFlags(readFileItem->flags() & ~Qt::ItemIsEditable);
    d->model->setItem(row, d->tableHdrNames.indexOf("read file"), readFileItem);

    resetRow(proxy_row);
    updateRow(proxy_row);
    addBytesTableRow(name);

    d->model->verticalHeaderItem(row)->setText(
        QString::number(d->model->rowCount()));
  }
}

void qColadaSegyReader::onRemoveToolBtnClicked() {
  Q_D(qColadaSegyReader);
  QModelIndexList indexList = d->tableView->selectionModel()->selectedRows();
  QList<int> rowList;

  for (QModelIndex &index : indexList)
    rowList.push_back(index.row());

  /* to remove rows correctly we should remove them in descending order */
  std::sort(rowList.rbegin(), rowList.rend()); // descending order sort

  for (int &row : rowList){
    QString readFile = d->proxy->data(
          d->proxy->index(row, 1))
        .toString();
    d->proxy->removeRow(row);
    removeBytesTableRow(readFile);
  }

  for (int row = 0; row < d->model->rowCount(); row++)
    d->model->verticalHeaderItem(row)->setText(QString::number(row + 1));
}

void qColadaSegyReader::onAutoDefineToolBtnClicked() {
  Q_D(qColadaSegyReader);
  QModelIndexList indexList = d->tableView->selectionModel()->selectedRows();
  for (QModelIndex &index : indexList)
    updateRow(index.row());
}

void qColadaSegyReader::onButtonBoxClicked(QAbstractButton *button) {
  Q_D(qColadaSegyReader);

  if (button == d->buttonBox->button(QDialogButtonBox::Ok)) {
    for (int i = 0; i < d->model->rowCount(); i++) {
      QString errMsg;
      SegyRead::ReadWriteParam p = getReadWriteParamFromTable(i, errMsg);
      if (!errMsg.isEmpty()) {
        QMessageBox::critical(this, "Error", errMsg);
        continue;
      }
      H5Seis_ptr seis(SegyRead::readTracesInHeap(
                   p, errMsg, getTrcHdrNamesFromBytesTable(p.readFile)));
      if (!errMsg.isEmpty() || !seis) {
        QMessageBox::critical(this, "Error", errMsg);
        continue;
      }
    }
  } else if (button == d->buttonBox->button(QDialogButtonBox::Cancel)) {
    this->close(); // close this widget
  }
}

void qColadaSegyReader::onBytesTableDataChanged(
    const QModelIndex &topLeft,
    const QModelIndex &bottomRight,
    const QVector<int> &roles)
{
  // length or `roles` independent from modified indexes
  // one index may contain multiple roles
  if (!roles.contains(Qt::DisplayRole) ||
      !topLeft.isValid())
    return;

  Q_D(qColadaSegyReader);
  QSortFilterProxyModel* proxy = dynamic_cast<QSortFilterProxyModel*>(
        d->trcHdrBytesTableView->model());
  QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(
        proxy->sourceModel());

  std::vector<std::string> trcHdrNames, trcHdrShortNames;
  h5geo::getTraceHeaderNames(trcHdrNames, trcHdrShortNames);

  QString data = model->data(topLeft).toString();
  bool isDataValid = std::find(
        trcHdrShortNames.begin(),
        trcHdrShortNames.end(),
        data.toStdString()) != trcHdrShortNames.end();
  if (isDataValid)
    model->itemFromIndex(topLeft)->setBackground(Qt::green);
  else
    model->itemFromIndex(topLeft)->setBackground(Qt::red);

  for (int col = 1; col < model->columnCount(); col++){
    if (topLeft.column() == col)
      continue;

    QModelIndex index = model->index(topLeft.row(), col);
    if (data == model->data(index).toString()){
      model->itemFromIndex(index)->setBackground(Qt::red);
    }
  }

  // check if any `red` item may become `green`
  for (int col_out = 1; col_out < model->columnCount(); col_out++){
    QModelIndex index_out = model->index(topLeft.row(), col_out);
    if (model->itemFromIndex(index_out)->background() != Qt::red)
      continue;

    bool recolor = true;
    for (int col_in = 1; col_in < model->columnCount(); col_in++){
      if (col_out == col_in)
        continue;

      QModelIndex index_in = model->index(topLeft.row(), col_in);
      if (model->data(index_out).toString() == model->data(index_in).toString()){
        recolor = false;
        break;
      }
    }
    if (recolor)
      model->itemFromIndex(index_out)->setBackground(Qt::green);
  }
}
