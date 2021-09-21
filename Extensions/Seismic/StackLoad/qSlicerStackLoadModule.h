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

#ifndef __qSlicerStackLoadModule_h
#define __qSlicerStackLoadModule_h

// Slicer includes
#include "qSlicerLoadableModule.h"

#include "qSlicerStackLoadModuleExport.h"

class qSlicerStackLoadModulePrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_STACKLOAD_EXPORT
qSlicerStackLoadModule
  : public qSlicerLoadableModule
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerStackLoadModule(QObject *parent=0);
  virtual ~qSlicerStackLoadModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  virtual QString helpText()const;
  virtual QString acknowledgementText()const;
  virtual QStringList contributors()const;

  virtual QIcon icon()const;

  virtual QStringList categories()const;
  virtual QStringList dependencies() const;

public slots:
  void onSeisModelDataChanged(
      const QModelIndex &topLeft,
      const QModelIndex &bottomRight,
      const QVector<int> &roles = QVector<int>());

  void onCubeAxesToolBtnToggled(bool val);

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();
  virtual void setupUI();
  virtual void setupUILogic();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerStackLoadModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerStackLoadModule);
  Q_DISABLE_COPY(qSlicerStackLoadModule);

};

#endif
