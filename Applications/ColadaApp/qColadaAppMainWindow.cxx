/*==============================================================================

  Copyright (c) Kitware, Inc.

  See http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware, Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Colada includes
#include "qColadaAppMainWindow.h"
#include "qColadaAppMainWindow_p.h"
#include "util.h"

// Qt includes
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QFile>
#include <QSettings>
#include <QShowEvent>
#include <QFormLayout>
#include <QLabel>

// CTK includes
#include <ctkFileDialog.h>
#include <ctkPopupWidget.h>
#include <ctkDirectoryButton.h>

// PythonQt includes
#include <PythonQt.h>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerAboutDialog.h"
#include "qSlicerMainWindow_p.h"
#include "qSlicerModuleSelectorToolBar.h"
#include "qSlicerLayoutManager.h"
#include "qSlicerSettingsStylesPanel.h"
#include "qSlicerSettingsGeneralPanel.h"
#include "qSlicerRelativePathMapper.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLSliceWidget.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLayoutNode.h"
#include "vtkStringArray.h"
#include "qMRMLThreeDViewControllerWidget.h"
#include "qMRMLThreeDView.h"

// VTK includes
#include "vtkMatrix3x3.h"

//-----------------------------------------------------------------------------
// qColadaAppMainWindowPrivate methods

qColadaAppMainWindowPrivate::qColadaAppMainWindowPrivate(qColadaAppMainWindow& object)
  : Superclass(object)
{
}

//-----------------------------------------------------------------------------
qColadaAppMainWindowPrivate::~qColadaAppMainWindowPrivate()
{
}

//-----------------------------------------------------------------------------
void qColadaAppMainWindowPrivate::init()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
  Q_Q(qColadaAppMainWindow);
  // Superclass::init() also initializes UI
  this->Superclass::init();
  setupPythonModules();

//  this->loadSeisDataFromDefaultDirectory();
//  this->loadWellDataFromDefaultDirectory();
//  this->loadMapDataFromDefaultDirectory();

  util::initProjLibDB();
}

void qColadaAppMainWindowPrivate::setupPythonModules() {
  PythonQtObjectPtr mainModule = PythonQt::self()->getMainModule();
  PythonQtObjectPtr colada_py =
      mainModule.evalScript(QString("import colada;"));
  //PythonQtObjectPtr h5gt_py =
  //    mainModule.evalScript(QString("import h5gtpy._h5gt;"));

  //PythonQtObjectPtr colada_py = mainModule.evalScript(QString("import colada;"));
  //PythonQtObjectPtr colada_py = PythonQt::self()->importModule(QString("colada"));
  //PythonQtObjectPtr h5gt_py = PythonQt::self()->importModule(QString("h5gtpy._h5gt"));
}

//-----------------------------------------------------------------------------
void qColadaAppMainWindowPrivate::setupUi(QMainWindow * mainWindow)
{
  Q_Q(qColadaAppMainWindow);

  qSlicerApplication * app = qSlicerApplication::application();
  if (!app){
    qCritical() << "qColadaAppMainWindowPrivate::setupUi: unable to get app instance";
    return;
  }

  //----------------------------------------------------------------------------
  // Add actions
  //----------------------------------------------------------------------------
  QAction* helpAboutSlicerAppAction = new QAction(mainWindow);
  helpAboutSlicerAppAction->setObjectName("HelpAboutColadaAppAction");
  helpAboutSlicerAppAction->setText("About " + app->applicationName());

  //----------------------------------------------------------------------------
  // Calling "setupUi()" after adding the actions above allows the call
  // to "QMetaObject::connectSlotsByName()" done in "setupUi()" to
  // successfully connect each slot with its corresponding action.
  this->Superclass::setupUi(mainWindow);
  
  // Add Help Menu Action
  this->HelpMenu->addAction(helpAboutSlicerAppAction);

  //----------------------------------------------------------------------------
  // Configure
  //----------------------------------------------------------------------------
  mainWindow->setWindowIcon(QIcon(":/Icons/Medium/DesktopIcon.png"));

//  QPixmap logo(":/LogoFull.png");
//  this->LogoLabel->setPixmap(logo);
//  this->LogoLabel->hide();

  // Hide the toolbars
  this->MainToolBar->setVisible(false);
  //this->ModuleSelectorToolBar->setVisible(false);
  this->ModuleToolBar->setVisible(false);
  this->ViewToolBar->setVisible(false);
  this->MouseModeToolBar->setVisible(false);
  this->CaptureToolBar->setVisible(false);
  this->ViewersToolBar->setVisible(false);
  this->DialogToolBar->setVisible(false);

  // Hide the menus
  //this->menubar->setVisible(false);
  //this->FileMenu->setVisible(false);
  //this->EditMenu->setVisible(false);
  //this->ViewMenu->setVisible(false);
  //this->LayoutMenu->setVisible(false);
  //this->HelpMenu->setVisible(false);

  // Hide the modules panel
  this->PanelDockWidget->setVisible(true);
  this->DataProbeCollapsibleWidget->setCollapsed(true);
  this->DataProbeCollapsibleWidget->setVisible(true);
  this->StatusBar->setVisible(true);

  // Setup Colada UI
  setupStatusBar(mainWindow);
  setupColadaStyles(mainWindow);
}

void qColadaAppMainWindowPrivate::setupStatusBar(QMainWindow* mainWindow) {
  currentProjectLabel = new QLabel(mainWindow);
  currentProjectLabel->setObjectName("CurrentProjectLabel");
  currentCRSLabel = new QLabel(mainWindow);
  currentCRSLabel->setObjectName("currentCRSLabel");

  StatusBar->addWidget(currentProjectLabel);
  StatusBar->addWidget(currentCRSLabel);
}

void qColadaAppMainWindowPrivate::setupColadaStyles(QMainWindow * mainWindow)
{
  Q_Q(qColadaAppMainWindow);
  qSlicerApplication * app = qSlicerApplication::application();
  if (!app){
    qCritical() << "qColadaAppMainWindowPrivate::setupColadaStyles: unable to get app instance";
    return;
  }

  qSlicerSettingsStylesPanel* settingsStylesPanel =
      qobject_cast<qSlicerSettingsStylesPanel*>(
        app->settingsDialog()->panel(qSlicerApplication::tr("Appearance")));

  if (!settingsStylesPanel){
    qCritical() << "qColadaAppMainWindowPrivate::setupColadaStyles: unable to get `qSlicerSettingsStylesPanel` to connect `currentStyleChanged`";
    return;
  }

  q->connect(settingsStylesPanel, &qSlicerSettingsStylesPanel::currentStyleChanged,
             q, &qColadaAppMainWindow::onCurrentStyleChanged);
}


//-----------------------------------------------------------------------------
// qColadaAppMainWindow methods

//-----------------------------------------------------------------------------
qColadaAppMainWindow::qColadaAppMainWindow(QWidget* windowParent)
  : Superclass(new qColadaAppMainWindowPrivate(*this), windowParent)
{
  Q_D(qColadaAppMainWindow);
  d->init();
}

//-----------------------------------------------------------------------------
qColadaAppMainWindow::qColadaAppMainWindow(
  qColadaAppMainWindowPrivate* pimpl, QWidget* windowParent)
  : Superclass(pimpl, windowParent)
{
  // init() is called by derived class.
}

//-----------------------------------------------------------------------------
qColadaAppMainWindow::~qColadaAppMainWindow()
{
}

//-----------------------------------------------------------------------------
void qColadaAppMainWindow::on_HelpAboutColadaAppAction_triggered()
{
  qSlicerAboutDialog about(this);
  about.setLogo(QPixmap(":/Logo.png"));
  about.exec();
}

void qColadaAppMainWindow::onCurrentStyleChanged(const QString& name)
{
  qSlicerApplication * app = qSlicerApplication::application();
  if (!app){
    qCritical() << "qColadaAppMainWindow::onCurrentStyleChanged: unable to get app instance";
    return;
  }

  if (QString::compare(name, "Light Colada", Qt::CaseInsensitive) == 0){
    QFile file(":/StyleSheets/ColadaLightStyle.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(file.readAll()) };
    app->setStyleSheet(styleSheet);
  } else if (QString::compare(name, "Dark Colada", Qt::CaseInsensitive) == 0){
    QFile file(":/StyleSheets/ColadaDarkStyle.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { QLatin1String(file.readAll()) };
    app->setStyleSheet(styleSheet);
  } else {
    app->setStyleSheet("");
  }
}

void qColadaAppMainWindow::showEvent(QShowEvent *event){
  Superclass::showEvent(event);

  if(event->spontaneous())
    return;

  // if Colada style is in app settings we need to initialize stylesheet
  this->onCurrentStyleChanged(qSlicerApplication::application()->style()->objectName());
}
