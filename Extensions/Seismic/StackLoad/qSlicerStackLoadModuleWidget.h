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

#ifndef __qSlicerStackLoadModuleWidget_h
#define __qSlicerStackLoadModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerStackLoadModuleExport.h"

class qSlicerStackLoadModuleWidgetPrivate;
class vtkMRMLNode;
class H5Seis;
class QStandardItem;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_STACKLOAD_EXPORT qSlicerStackLoadModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

  struct NodeParam {
    QString h5FileName, seisName, nodeName;
    double ilFrom, ilTo, xlFrom, xlTo, sampFrom, sampTo;
  };

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerStackLoadModuleWidget(QWidget *parent=0);
  virtual ~qSlicerStackLoadModuleWidget();

public slots:
  bool addSeis(H5Seis* seis);
  bool removeSeis(H5Seis* seis);
  NodeParam getNodeParamFromTable(int proxy_row);
  void updateRangeWidgets(int proxy_row);
  void onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous);
  void onUploadBtnClicked();

  QVector<int> findIntersectedRows(
      const QList<QStandardItem*>& list_1,
      const QList<QStandardItem*>& list_2);
  std::vector<std::string> getAllDataFromColumn(const QString& colName);

protected:
  QScopedPointer<qSlicerStackLoadModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerStackLoadModuleWidget);
  Q_DISABLE_COPY(qSlicerStackLoadModuleWidget);
};

#endif
