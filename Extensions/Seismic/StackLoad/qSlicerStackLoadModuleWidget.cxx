/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>

// Slicer includes
#include "qSlicerStackLoadModuleWidget.h"
#include "ui_qSlicerStackLoadModuleWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerModuleSelectorToolBar.h"
#include "qSlicerModulesMenu.h"

// ctk includes
#include <ctkRangeWidget.h>
#include <ctkDoubleSpinBox.h>
#include <ctkCheckableHeaderView.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkCubeAxesActor.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLVolumeRenderingDisplayNode.h>
#include <MRMLDisplayableManagerModule.h>

// Colada includes
#include "qColadaAppMainWindow.h"
#include "qColadaH5SeisTreeView.h"
#include "qColadaH5SeisModel.h"
#include "qColadaH5ProxyModel.h"
#include "qColadaH5Item.h"

// h5geo includes
#include <h5geo/h5core.h>
#include <h5geo/h5seis.h>
#include <h5geo/h5seiscontainer.h>

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

#include "vtkEigenFloatArray.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerStackLoadModuleWidgetPrivate: public Ui_qSlicerStackLoadModuleWidget
{
public:
  qSlicerStackLoadModuleWidgetPrivate();

  QStringList tableHdrNames = QStringList(
        {"h5 file", "seis name", "node name",
         "IL from", "IL to",
         "XL from", "XL to",
         "samp from", "samp to"});

  QStringList tableHdrTips = QStringList(
        {"H5 container name", "Seis name within container", "Node name in the scene",
         "First Inline", "Last ILine",
         "First XLine", "Last XLine",
         "First Sample", "Last Sample"});

  QSortFilterProxyModel* proxy;
  QStandardItemModel* model;
};

//-----------------------------------------------------------------------------
// qSlicerStackLoadModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerStackLoadModuleWidgetPrivate::qSlicerStackLoadModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerStackLoadModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerStackLoadModuleWidget::qSlicerStackLoadModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerStackLoadModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerStackLoadModuleWidget::~qSlicerStackLoadModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerStackLoadModuleWidget::setup()
{
  Q_D(qSlicerStackLoadModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  d->tableView->setObjectName("StackLoadTableView");
  d->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  d->tableView->setAlternatingRowColors(true);
  d->tableView->setAcceptDrops(false);
  d->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  d->tableView->setSortingEnabled(true);
  ctkCheckableHeaderView* hrzHeader = new ctkCheckableHeaderView(Qt::Horizontal, d->tableView);
  hrzHeader->setObjectName("StackLoadHrzHeader");
  hrzHeader->setSectionsClickable(true);
  hrzHeader->setSectionsMovable(false);
  hrzHeader->setHighlightSections(true);
  d->tableView->setHorizontalHeader(hrzHeader);
  d->proxy = new QSortFilterProxyModel();
  d->model = new QStandardItemModel();
  d->proxy->setSourceModel(d->model);
  d->tableView->setModel(d->proxy);

  d->model->setColumnCount(d->tableHdrNames.count());
  d->model->setHorizontalHeaderLabels(d->tableHdrNames);
  for (int i = 0; i < d->model->columnCount(); i++) {
    d->model->horizontalHeaderItem(i)->setToolTip(d->tableHdrTips[i]);
  }

  d->ilRangeWidget->setDecimals(0);
  d->xlRangeWidget->setDecimals(0);
  d->sampRangeWidget->setDecimals(0);

  connect(
        d->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
        this, &qSlicerStackLoadModuleWidget::onCurrentRowChanged);
  connect(
        d->uploadBtn, &QAbstractButton::clicked,
        this, &qSlicerStackLoadModuleWidget::onUploadBtnClicked);
}

bool qSlicerStackLoadModuleWidget::addSeis(H5Seis* seis)
{
  Q_D(qSlicerStackLoadModuleWidget);

  if (seis == nullptr)
    return false;

  QString fileName = QString::fromStdString(seis->getObjG().getFileName());
  QString seisName = QString::fromStdString(seis->getObjG().getPath());

  QList<QStandardItem*> fileItemList = d->model->findItems(
        fileName, Qt::MatchFixedString, d->tableHdrNames.indexOf("h5 file"));

  QList<QStandardItem*> seisItemNameList = d->model->findItems(
        seisName, Qt::MatchFixedString, d->tableHdrNames.indexOf("seis name"));

  QVector<int> rows = findIntersectedRows(fileItemList, seisItemNameList);

  if (!rows.isEmpty())
    return false;

  d->model->insertRow(d->model->rowCount());
  int row = d->model->rowCount() - 1;
  int proxy_row = d->proxy->mapFromSource(
        d->model->index(row, 0)).row();

  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("h5 file")),
                    fileName);
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("seis name")),
                    seisName);
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("IL from")),
                    seis->getTraceHeaderMin("INLINE"));
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("IL to")),
                    seis->getTraceHeaderMax("INLINE"));
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("XL from")),
                    seis->getTraceHeaderMin("XLINE"));
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("XL to")),
                    seis->getTraceHeaderMax("XLINE"));
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("samp from")),
                    seis->getFirstSample(0));
  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("samp to")),
                    seis->getLastSample(0));

  std::vector<std::string> nodeNames = getAllDataFromColumn("node name");
  std::string nodeName;
  h5geo::splitPathToParentAndObj(seisName.toStdString(), nodeName);  
  nodeName = mrmlScene()->GenerateUniqueName(nodeName);

  d->model->setData(d->model->index(row, d->tableHdrNames.indexOf("node name")),
                    QString::fromStdString(nodeName));

  d->tableView->selectionModel()->select(
        d->tableView->model()->index(row, 0),
        QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);

  updateRangeWidgets(proxy_row);

  return true;
}

bool qSlicerStackLoadModuleWidget::removeSeis(H5Seis* seis)
{
  Q_D(qSlicerStackLoadModuleWidget);

  if (seis == nullptr)
    return false;

  QString fileName = QString::fromStdString(seis->getObjG().getFileName());
  QString seisName = QString::fromStdString(seis->getObjG().getPath());

  QList<QStandardItem*> fileItemList = d->model->findItems(
        fileName, Qt::MatchFixedString, d->tableHdrNames.indexOf("h5 file"));

  QList<QStandardItem*> seisItemNameList = d->model->findItems(
        seisName, Qt::MatchFixedString, d->tableHdrNames.indexOf("seis name"));

  QVector<int> rows = findIntersectedRows(fileItemList, seisItemNameList);

  if (rows.isEmpty())
    return false;

  for (const auto& row : rows)
    d->model->removeRow(row);

  return true;
}

qSlicerStackLoadModuleWidget::NodeParam
qSlicerStackLoadModuleWidget::getNodeParamFromTable(int proxy_row)
{
  Q_D(qSlicerStackLoadModuleWidget);

  if (proxy_row < 0)
    return qSlicerStackLoadModuleWidget::NodeParam();

  qSlicerStackLoadModuleWidget::NodeParam p;
  p.h5FileName = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("h5 file")))
            .toString();
  p.seisName = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("seis name")))
            .toString();
  p.nodeName = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("node name")))
            .toString();
  p.ilFrom = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("IL from")))
            .toDouble();
  p.ilTo = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("IL to")))
            .toDouble();
  p.xlFrom = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("XL from")))
            .toDouble();
  p.xlTo = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("XL to")))
            .toDouble();
  p.sampFrom = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("samp from")))
            .toDouble();
  p.sampTo = d->proxy->data(
        d->proxy->index(proxy_row, d->tableHdrNames.indexOf("samp to")))
            .toDouble();

  return p;
}

void qSlicerStackLoadModuleWidget::updateRangeWidgets(int proxy_row)
{
  Q_D(qSlicerStackLoadModuleWidget);

  qSlicerStackLoadModuleWidget::NodeParam p = getNodeParamFromTable(proxy_row);
  std::string file = p.h5FileName.toStdString();
  std::string seisName = p.seisName.toStdString();
  H5SeisCnt_ptr seisCnt =
      H5SeisCnt_ptr(h5geo::openSeisContainerByName(file));
  if (!seisCnt)
    return;

  H5Seis_ptr seis = H5Seis_ptr(seisCnt->getSeis(seisName));
  if (!seis)
    return;

  d->ilRangeWidget->setRange(seis->getTraceHeaderMin("INLINE"), seis->getTraceHeaderMax("INLINE"));
  d->xlRangeWidget->setRange(seis->getTraceHeaderMin("XLINE"), seis->getTraceHeaderMax("XLINE"));
  d->sampRangeWidget->setRange(seis->getFirstSample(0), seis->getLastSample(0));

  d->ilRangeWidget->setMinimumValue(p.ilFrom);
  d->xlRangeWidget->setMinimumValue(p.xlFrom);
  d->sampRangeWidget->setMinimumValue(p.sampFrom);

  d->ilRangeWidget->setMaximumValue(p.ilTo);
  d->xlRangeWidget->setMaximumValue(p.xlTo);
  d->sampRangeWidget->setMaximumValue(p.sampTo);

}

void qSlicerStackLoadModuleWidget::onCurrentRowChanged(
    const QModelIndex &current, const QModelIndex &previous)
{
  updateRangeWidgets(current.row());
}

QVector<int> qSlicerStackLoadModuleWidget::findIntersectedRows(
    const QList<QStandardItem*>& list_1,
    const QList<QStandardItem*>& list_2)
{
  QVector<int> rows_1(list_1.count());
  QVector<int> rows_2(list_2.count());

  for (int i = 0; i < list_1.count(); i++)
    rows_1[i] = list_1[i]->row();

  for (int i = 0; i < list_2.count(); i++)
    rows_2[i] = list_2[i]->row();

  std::sort(rows_1.begin(), rows_1.end());
  std::sort(rows_2.begin(), rows_2.end());

  QVector<int> v(rows_1.count() + rows_2.count());
  QVector<int>::iterator end = std::set_intersection(
        rows_1.begin(), rows_1.end(),
        rows_2.begin(), rows_2.end(),
        v.begin());

  v.resize(end-v.begin());
  return v;
}

std::vector<std::string> qSlicerStackLoadModuleWidget::getAllDataFromColumn(
    const QString& colName)
{
  Q_D(qSlicerStackLoadModuleWidget);

  std::vector<std::string> str_v(d->model->rowCount());
  int col = d->tableHdrNames.indexOf(colName);
  for (int row = 0; row < d->model->rowCount(); row++){
    str_v[row] = d->model->data(d->model->index(row, col)).
        toString().toStdString();
  }

  return str_v;
}

void qSlicerStackLoadModuleWidget::onUploadBtnClicked()
{
  Q_D(qSlicerStackLoadModuleWidget);

  if (!d->tableView->selectionModel()->hasSelection())
    return;

  QModelIndexList indexList = d->tableView->selectionModel()->selectedRows();

  for (const auto& index : indexList){
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("IL from")),
                      d->ilRangeWidget->minimumValue());
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("IL to")),
                      d->ilRangeWidget->maximumValue());
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("XL from")),
                      d->xlRangeWidget->minimumValue());
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("XL to")),
                      d->xlRangeWidget->maximumValue());
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("samp from")),
                      d->sampRangeWidget->minimumValue());
    d->proxy->setData(d->proxy->index(index.row(), d->tableHdrNames.indexOf("samp to")),
                      d->sampRangeWidget->maximumValue());

    auto p = getNodeParamFromTable(index.row());
    std::string file = p.h5FileName.toStdString();
    std::string seisName = p.seisName.toStdString();
    H5SeisCnt_ptr seisCnt =
        H5SeisCnt_ptr(h5geo::openSeisContainerByName(file));
    if (!seisCnt)
      continue;

    H5Seis_ptr seis = H5Seis_ptr(seisCnt->getSeis(seisName));
    if (!seis)
      continue;

    Eigen::VectorXd samples = seis->getSamples(0);
    Eigen::VectorX<bool> boolInd =
        samples.array() >= p.sampFrom &&
        samples.array() <= p.sampTo;
    Eigen::VectorX<ptrdiff_t> linInd = h5geo::find_index(boolInd);
    samples = samples(linInd);

    vtkNew<vtkEigenFloatArray> TRACE;
    Eigen::MatrixXd HDR;
    seis->getSortedData(
          *TRACE->GetEigenArray(), HDR, {"INLINE", "XLINE"},
          {p.ilFrom, p.xlFrom}, {p.ilTo, p.xlTo},
          samples(0), samples.size());

    TRACE->UpdateEigenBuffer();

    auto uvalG = seis->getUValG();
    if (!uvalG.has_value())
      return;

    size_t n_elem;
    Eigen::VectorXd uIL, uXL;
    n_elem = uvalG->getDataSet("INLINE").getMemSpace().getElementCount();
    uIL.resize(n_elem);
    uvalG->getDataSet("INLINE").read(uIL.data());
    n_elem = uvalG->getDataSet("XLINE").getMemSpace().getElementCount();
    uXL.resize(n_elem);
    uvalG->getDataSet("XLINE").read(uXL.data());

    boolInd =
        uIL.array() >= p.ilFrom &&
        uIL.array() <= p.ilTo;
    linInd = h5geo::find_index(boolInd);
    uIL = uIL(linInd);

    boolInd =
        uXL.array() >= p.xlFrom &&
        uXL.array() <= p.xlTo;
    linInd = h5geo::find_index(boolInd);
    uXL = uXL(linInd);

    h5gt::Group objG = seis->getObjG();
    double orientation;
    Eigen::VectorXd origin(objG.getAttribute("origin").getMemSpace().getElementCount());
    objG.getAttribute("orientation").read(orientation);
    objG.getAttribute("origin").read(origin.data());

    vtkNew<vtkImageData> imageData;
    imageData->SetOrigin(0, 0, 0);
    imageData->SetSpacing(1, 1, 1);
    imageData->SetDimensions(samples.size(), uXL.size(), uIL.size());
    imageData->AllocateScalars(VTK_FLOAT, 1);
    imageData->GetPointData()->SetScalars(TRACE);

    vtkNew<vtkTransform> transform;
    transform->RotateY(90);

    vtkNew<vtkMRMLScalarVolumeNode> volumeNode;
//    vtkNew<vtkMRMLVolumeNode> volumeNode;
    volumeNode->SetName(p.nodeName.toUtf8());
    volumeNode->SetAndObserveImageData(imageData);
    volumeNode->SetOrigin(0, 0, 0);
    volumeNode->SetSpacing(1, 1, -1);
//    volumeNode->ApplyTransformMatrix(transform->GetMatrix());
    volumeNode->ApplyTransform(transform);
    mrmlScene()->AddNode(volumeNode.GetPointer());
    volumeNode->CreateDefaultDisplayNodes();
  }
}
