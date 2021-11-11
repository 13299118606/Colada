/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QAction>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QtGlobal>
#include <QMainWindow>
#include <QDialog>
#include <QRandomGenerator>
#include <QSurfaceFormat>
#include <QSysInfo>
#include <QTextCodec>
#include <QVBoxLayout>

#if defined(Q_OS_WIN32)
  #include <QtPlatformHeaders\QWindowsWindowFunctions> // for setHasBorderInFullScreen
#endif

#include "vtkSlicerConfigure.h" // For Slicer_USE_*, Slicer_BUILD_*_SUPPORT

// CTK includes
#include <ctkColorDialog.h>
#include <ctkErrorLogModel.h>
#include <ctkErrorLogFDMessageHandler.h>
#include <ctkErrorLogQtMessageHandler.h>
#include <ctkErrorLogStreamMessageHandler.h>
#include <ctkITKErrorLogMessageHandler.h>
#include <ctkMessageBox.h>
#ifdef Slicer_USE_PYTHONQT
# include "ctkPythonConsole.h"
#endif
#include <ctkSettings.h>
#ifdef Slicer_USE_QtTesting
#include <ctkQtTestingUtility.h>
#include <ctkXMLEventObserver.h>
#include <ctkXMLEventSource.h>
#endif
#ifdef Slicer_BUILD_DICOM_SUPPORT
#include <ctkDICOMBrowser.h>
#endif
#include <ctkToolTipTrapper.h>
#include <ctkVTKErrorLogMessageHandler.h>

// QTGUI includes
#include "qSlicerAbstractModule.h"
#include "qSlicerAbstractModuleRepresentation.h"
#include "qColadaApplication.h"
#include "qSlicerCommandOptions.h"
#include "qSlicerCoreApplication_p.h"
#include "qSlicerIOManager.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerModuleFactoryManager.h"
#include "qSlicerModuleManager.h"
#ifdef Slicer_USE_PYTHONQT
# include "qSlicerPythonManager.h"
# include "qSlicerSettingsPythonPanel.h"
#endif
#ifdef Slicer_BUILD_EXTENSIONMANAGER_SUPPORT
# include "qSlicerExtensionsManagerDialog.h"
# include "qSlicerSettingsExtensionsPanel.h"
#endif
#include "qSlicerSettingsCachePanel.h"
#include "qSlicerSettingsGeneralPanel.h"
#ifdef Slicer_BUILD_I18N_SUPPORT
# include "qSlicerSettingsInternationalizationPanel.h"
#endif
#include "qSlicerSettingsModulesPanel.h"
#include "qSlicerSettingsStylesPanel.h"
#include "qSlicerSettingsViewsPanel.h"
#include "qSlicerSettingsDeveloperPanel.h"
#include "qSlicerSettingsUserInformationPanel.h"

// qMRMLWidget includes
#include "qMRMLEventBrokerConnection.h"

// qMRML includes
#ifdef Slicer_USE_QtTesting
#include <qMRMLCheckableNodeComboBoxEventPlayer.h>
#include <qMRMLNodeComboBoxEventPlayer.h>
#include <qMRMLNodeComboBoxEventTranslator.h>
#include <qMRMLTreeViewEventPlayer.h>
#include <qMRMLTreeViewEventTranslator.h>
#endif

// Logic includes
#include <vtkSlicerApplicationLogic.h>
#include <vtkSystemInformation.h>

// MRML includes
#include <vtkMRMLMessageCollection.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkNew.h>
#include <vtkVersionMacros.h> // must precede reference to VTK_*_VERSION
#if VTK_MAJOR_VERSION >= 9
#include <vtkSMP.h> // For VTK_SMP_BACKEND
#endif

//-----------------------------------------------------------------------------
class qColadaApplicationPrivate
{
  Q_DECLARE_PUBLIC(qColadaApplication);
protected:
  qColadaApplication* const q_ptr;
public:
  qColadaApplicationPrivate(qColadaApplication& object);
  virtual ~qColadaApplicationPrivate();

  /// Convenient method regrouping all initialization code
  virtual void init();
};


//-----------------------------------------------------------------------------
// qColadaApplicationPrivate methods

//-----------------------------------------------------------------------------
qColadaApplicationPrivate::qColadaApplicationPrivate(
    qColadaApplication& object) : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qColadaApplicationPrivate::~qColadaApplicationPrivate()
{
}

//-----------------------------------------------------------------------------
void qColadaApplicationPrivate::init()
{
  Q_Q(qColadaApplication);
}

//-----------------------------------------------------------------------------
// qColadaApplication methods

//-----------------------------------------------------------------------------
qColadaApplication::qColadaApplication(int &_argc, char **_argv)
  : Superclass(_argc, _argv),
    d_ptr(new qColadaApplicationPrivate(*this))
{
  Q_D(qColadaApplication);
  d->init();
}

//-----------------------------------------------------------------------------
qColadaApplication::~qColadaApplication()
{

}

//-----------------------------------------------------------------------------
qColadaApplication* qColadaApplication::application()
{
  qColadaApplication* app = qobject_cast<qColadaApplication*>(QApplication::instance());
  return app;
}

QString qColadaApplication::getCRSName()
{
  QSettings* appSettings = this->userSettings();
  Q_ASSERT(appSettings);

  QString crsName = appSettings->value("CRSName", "").toString();
  return crsName;
}
