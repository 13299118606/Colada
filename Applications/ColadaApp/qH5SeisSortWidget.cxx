// h5geo includes must be included before _p.h private file
// as it contains declaration on H5Seis_ptr
#include <h5geo/h5core.h>
#include <h5geo/h5seiscontainer.h>
#include <h5geo/h5seis.h>

// Colada includes
#include "qH5SeisSortWidget.h"
// private header must be included after h5seis
// it stores
#include "qH5SeisSortWidget_p.h"
#include "qCompletableComboBox.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

// CTK includes
#include <ctkRangeWidget.h>

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>


qH5SeisSortWidgetPrivate::qH5SeisSortWidgetPrivate(qH5SeisSortWidget &q)
    : q_ptr(&q) {}

qH5SeisSortWidgetPrivate::~qH5SeisSortWidgetPrivate() {}

void qH5SeisSortWidgetPrivate::init() {
  Q_Q(qH5SeisSortWidget);
  mainVLayout = new QVBoxLayout(q);
  addRemoveSortHLayout = new QHBoxLayout;

  QWidget* w = q->createWidgetPair();
  pKeyComboBox = w->findChild<qCompletableComboBox*>();
  pKeyRangeWidget = w->findChild<ctkRangeWidget*>();

  addSortBtn = new QToolButton;
  removeSortBtn = new QToolButton;

  addSortBtn->setIcon(QIcon(":/Icons/Add.png"));
  removeSortBtn->setIcon(QIcon(":/Icons/Remove.png"));

  addRemoveSortHLayout->addWidget(addSortBtn, 0, Qt::AlignLeft);
  addRemoveSortHLayout->addWidget(removeSortBtn, 0, Qt::AlignLeft);
  addRemoveSortHLayout->addStretch(1);

  mainVLayout->addWidget(w);
  mainVLayout->addLayout(addRemoveSortHLayout);

  q->connect(pKeyComboBox, &qCompletableComboBox::currentTextChanged,
          q, &qH5SeisSortWidget::onSortComboCurrentTextChanged);
  q->connect(addSortBtn, &QToolButton::clicked,
          q, &qH5SeisSortWidget::onAddSortBtnClicked);
  q->connect(removeSortBtn, &QToolButton::clicked,
          q, &qH5SeisSortWidget::onRemoveSortBtnClicked);
}

qH5SeisSortWidget::qH5SeisSortWidget(QWidget *parent)
  : QWidget(parent), d_ptr(new qH5SeisSortWidgetPrivate(*this)) {
  Q_D(qH5SeisSortWidget);
  d->init();
}

qH5SeisSortWidget::qH5SeisSortWidget(qH5SeisSortWidgetPrivate *pimpl, QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5SeisSortWidget::~qH5SeisSortWidget() {

}

QWidget* qH5SeisSortWidget::createWidgetPair(){
  QWidget* widget = new QWidget;
  qCompletableComboBox* combo = new qCompletableComboBox;
  ctkRangeWidget* rangeWidget = new ctkRangeWidget;
  combo->setObjectName("SortComboBox");
  rangeWidget->setObjectName("SortRangeWidget");

  QHBoxLayout* hLayout = new QHBoxLayout(widget);
  hLayout->setMargin(0);
  hLayout->addWidget(combo);
  hLayout->addWidget(rangeWidget);
  return widget;
}

void qH5SeisSortWidget::setSeis(H5Seis* seis)
{
  Q_D(qH5SeisSortWidget);
  if (!seis){
    return;
  }

  d->traceHeadersMin = seis->getTraceHeaderMin();
  d->traceHeadersMax = seis->getTraceHeaderMax();
  d->pKeyNames = seis->getPKeyNames();

  this->updateWidgets();
}

void qH5SeisSortWidget::updateWidgets(){
  Q_D(qH5SeisSortWidget);
  d->pKeyComboBox->clear();

  for (const auto& name : d->pKeyNames)
    d->pKeyComboBox->addItem(QString::fromStdString(name));
  d->pKeyComboBox->model()->sort(0, Qt::AscendingOrder);

  for (int i = 0; i < d->mainVLayout->count(); i++){
    auto item = d->mainVLayout->itemAt(i);
    if (!item || !item->widget())
      continue;

    ctkRangeWidget* rangeWidget =
        item->widget()->findChild<ctkRangeWidget*>();
    if (!rangeWidget)
      continue;

    this->updateRangeWidget(rangeWidget);
  }
}

void qH5SeisSortWidget::updateRangeWidget(ctkRangeWidget* rangeWidget){
  Q_D(qH5SeisSortWidget);
  if (!rangeWidget)
    return;

  QWidget* w = qobject_cast<QWidget*>(rangeWidget->parent());
  if (!w)
    return;

  qCompletableComboBox* combo = w->findChild<qCompletableComboBox*>();
  if (!combo)
    return;

  std::string key = combo->currentText().toStdString();
  double keyMin = NAN, keyMax = NAN;
  if (d->traceHeadersMin.count(key) > 0)
    keyMin = d->traceHeadersMin.at(key);

  if (d->traceHeadersMax.count(key) > 0)
    keyMax = d->traceHeadersMax.at(key);

  rangeWidget->setRange(keyMin, keyMax);
  rangeWidget->setValues(keyMin, keyMax);
}

void qH5SeisSortWidget::onAddSortBtnClicked(bool val){
  Q_D(qH5SeisSortWidget);
  QWidget* w = this->createWidgetPair();
  qCompletableComboBox* combo =
      w->findChild<qCompletableComboBox*>();
  ctkRangeWidget* rangeWidget =
      w->findChild<ctkRangeWidget*>();
  this->addKeysModelToComboBox(combo);
  this->updateRangeWidget(rangeWidget);
  d->mainVLayout->insertWidget(d->mainVLayout->count()-1, w);

  connect(combo, &qCompletableComboBox::currentTextChanged,
          this, &qH5SeisSortWidget::onSortComboCurrentTextChanged);
}

void qH5SeisSortWidget::onRemoveSortBtnClicked(bool val){
  Q_D(qH5SeisSortWidget);
  if (d->mainVLayout->count() == 2)
    return;

  auto item = d->mainVLayout->itemAt(d->mainVLayout->count()-2);
  if (!item)
    return;

  delete item->widget();
}

void qH5SeisSortWidget::addKeysModelToComboBox(QComboBox* combo){
  std::vector<std::string> fullHdrNames, shortHdrNames;
  h5geo::getTraceHeaderNames(fullHdrNames, shortHdrNames);

  combo->clear();
  for (size_t i = 0; i < shortHdrNames.size(); i++){
    combo->insertItem(i, QString::fromStdString(shortHdrNames[i]));
    combo->setItemData(i, QString::fromStdString(fullHdrNames[i]), Qt::ToolTipRole);
  }
  combo->model()->sort(0, Qt::AscendingOrder);

  QModelIndexList indexList =
      combo->model()->match(combo->model()->index(0, 0), Qt::DisplayRole, "XLINE", 1);
  if (!indexList.isEmpty())
    combo->setCurrentIndex(indexList[0].row());
}

void qH5SeisSortWidget::onSortComboCurrentTextChanged(const QString &text){
  Q_D(qH5SeisSortWidget);
  qCompletableComboBox* combo =
      qobject_cast<qCompletableComboBox*>(sender());
  if (!combo)
    return;

  QWidget* w = qobject_cast<QWidget*>(combo->parent());
  if (!w)
    return;

  int layoutInd = d->mainVLayout->indexOf(w);
  if (layoutInd < 0)
    return;

  ctkRangeWidget* rangeWidget = w->findChild<ctkRangeWidget*>();
  if (!rangeWidget)
    return;

  this->updateRangeWidget(rangeWidget);
}

QStringList qH5SeisSortWidget::getHeadersLimits(
    QList<double>& minList, QList<double>& maxList)
{
  Q_D(qH5SeisSortWidget);
  QStringList headerList;
  minList.clear();
  maxList.clear();
  for (int i = 0; i < d->mainVLayout->count(); i++){
    auto item = d->mainVLayout->itemAt(i);
    if (!item || !item->widget())
      continue;

    qCompletableComboBox* combo =
        item->widget()->findChild<qCompletableComboBox*>();
    ctkRangeWidget* rangeWidget =
        item->widget()->findChild<ctkRangeWidget*>();
    if (!combo || !rangeWidget)
      continue;

    headerList.append(combo->currentText());
    minList.append(rangeWidget->minimumValue());
    maxList.append(rangeWidget->maximumValue());
  }

  // if for some reason their sizes are not equal then return empty
  if (headerList.count() != minList.count() ||
      headerList.count() != maxList.count() ||
      minList.count() != maxList.count()){
    headerList.clear();
    minList.clear();
    maxList.clear();
  }

  return headerList;
}
