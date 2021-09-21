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
#include "qColadaNewProject.h"
#include "qColadaH5SurfTreeView.h"
#include "qColadaH5SeisTreeView.h"
#include "qColadaH5WellTreeView.h"
#include "qColadaH5SurfModel.h"
#include "qColadaH5SeisModel.h"
#include "qColadaH5WellModel.h"
#include "qColadaH5ProxyModel.h"
#include "ColadaDBCore.h"

// Qt includes
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QToolButton>

// CTK includes
#include <ctkFileDialog.h>
#include <ctkPopupWidget.h>

// PythonQt includes
#include <PythonQt.h>

// Slicer includes
#include "qSlicerApplication.h"
#include "qSlicerAboutDialog.h"
#include "qSlicerMainWindow_p.h"
#include "qSlicerModuleSelectorToolBar.h"
#include "qSlicerLayoutManager.h"
#include "qMRMLThreeDWidget.h"
#include "qMRMLSliceWidget.h"
#include "vtkMRMLViewNode.h"
#include "vtkMRMLSliceNode.h"
#include "qMRMLThreeDViewControllerWidget.h"
#include "qMRMLThreeDView.h"

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
  this->Superclass::init();
  setupPythonModules();
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
  qSlicerApplication * app = qSlicerApplication::application();

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

  QPixmap logo(":/LogoFull.png");
  this->LogoLabel->setPixmap(logo);

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
  this->StatusBar->setStyleSheet(
      "QStatusBar::item { border: 1px solid red; border-radius: 3px; } ");

  // Setup Colada UI
  setupDockWidgets(mainWindow);
  setupTreeViews(mainWindow);
  setupStatusBar(mainWindow);
  setupMenuBar(mainWindow);
}

void qColadaAppMainWindowPrivate::setupDockWidgets(QMainWindow* mainWindow) {
  PanelDockWidget->setWindowTitle("Modules");

  seisDockWidget = new QDockWidget("Seismic", mainWindow);
  surfDockWidget = new QDockWidget("Surface", mainWindow);
  wellDockWidget = new QDockWidget("Well", mainWindow);

  seisDockWidget->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                                  Qt::DockWidgetArea::RightDockWidgetArea);
  surfDockWidget->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                                  Qt::DockWidgetArea::RightDockWidgetArea);
  wellDockWidget->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea |
                                  Qt::DockWidgetArea::RightDockWidgetArea);

  mainWindow->addDockWidget(Qt::LeftDockWidgetArea, seisDockWidget);
  seisDockWidget->setObjectName("SeisDockWidget");
  mainWindow->addDockWidget(Qt::LeftDockWidgetArea, surfDockWidget);
  surfDockWidget->setObjectName("SurfDockWidget");
  mainWindow->addDockWidget(Qt::LeftDockWidgetArea, wellDockWidget);
  wellDockWidget->setObjectName("WellDockWidget");

  mainWindow->tabifyDockWidget(wellDockWidget, surfDockWidget);
  mainWindow->tabifyDockWidget(surfDockWidget, seisDockWidget);
  mainWindow->tabifyDockWidget(seisDockWidget, PanelDockWidget);

  std::vector<std::string> axesNames({"X", "-X", "-Y", "Y", "-Z", "Z"});

  for (int j = 0; j < this->LayoutManager->threeDViewCount(); j++)
    for (int i = 0; i < axesNames.size(); i++)
      this->LayoutManager->threeDWidget(j)->mrmlViewNode()->SetAxisLabel(
          i, axesNames[i].c_str());

  for (const QString& sliceViewName : this->LayoutManager->sliceViewNames())
    for (int i = 0; i < axesNames.size(); i++)
      this->LayoutManager->sliceWidget(sliceViewName)
          ->mrmlSliceNode()
          ->SetAxisLabel(i, axesNames[i].c_str());


  //this->LayoutManager->sliceWidget("")->sliceController()

  //QPushButton *btn = new QPushButton(
  //    "SUKA", LayoutManager->threeDWidget(0)->threeDController());
  //LayoutManager->threeDWidget(0)->threeDController()->barLayout()->addWidget(
  //    btn);

  //QGridLayout *gridLayout = qobject_cast<QGridLayout *>(
  //    LayoutManager->threeDWidget(0)->threeDController()->layout());
  //gridLayout->addWidget(btn);
  //QLayout *layout =
  //    LayoutManager->threeDWidget(0)->threeDController()->layout();
  //layout->addWidget(btn);

  LayoutManager->threeDWidget(0)->threeDController();

  //LayoutManager->threeDWidget(0)
  //    ->threeDController()
  //    ->pinButton()
  //    ->layout()
  //    ->addWidget(btn);
  
  LayoutManager->threeDWidget(0)->threeDController()->setWindowTitle("MyTitle");
  LayoutManager->threeDWidget(0)->threeDController()->setToolTip("MyToolTip");

  //ctkPopupWidget* popupWidget = qobject_cast<ctkPopupWidget *>(
  //    LayoutManager->threeDWidget(0)->threeDController()->pinButton());

  LayoutManager->threeDWidget(0)->threeDController()->pinButton();

  //QWidget* w = popupWidget->baseWidget();
  //w->layout()->addWidget(btn);

  LayoutManager->threeDWidget(0)->threeDView()->displayableManagerByClassName(
      "vtkMRMLModelDisplayableManager");

  LayoutManager->threeDWidget(0)->threeDController()->barWidget()->setToolTip(
      "BarWidget");

  vtkMRMLAbstractDisplayableManager* modelDisplayableManager =
      LayoutManager->threeDWidget(0)
          ->threeDView()
          ->displayableManagerByClassName("vtkMRMLModelDisplayableManager");
}

void qColadaAppMainWindowPrivate::setupTreeViews(QMainWindow *mainWindow) {
  surfTreeView = new qColadaH5SurfTreeView(surfDockWidget);
  seisTreeView = new qColadaH5SeisTreeView(seisDockWidget);
  wellTreeView = new qColadaH5WellTreeView(wellDockWidget);

  surfDockWidget->setWidget(surfTreeView);
  seisDockWidget->setWidget(seisTreeView);
  wellDockWidget->setWidget(wellTreeView);

  qColadaH5SurfModel *surfModel = new qColadaH5SurfModel("Surface tree model");
  qColadaH5SeisModel *seisModel = new qColadaH5SeisModel("Seismic tree model");
  qColadaH5WellModel *wellModel = new qColadaH5WellModel("Well tree model");

  qColadaH5ProxyModel *surfProxy = new qColadaH5ProxyModel(surfTreeView);
  qColadaH5ProxyModel *seisProxy = new qColadaH5ProxyModel(seisTreeView);
  qColadaH5ProxyModel *wellProxy = new qColadaH5ProxyModel(wellTreeView);

  surfProxy->setSourceModel(surfModel);
  seisProxy->setSourceModel(seisModel);
  wellProxy->setSourceModel(wellModel);

  surfTreeView->setModel(surfProxy);
  seisTreeView->setModel(seisProxy);
  wellTreeView->setModel(wellProxy);
}

void qColadaAppMainWindowPrivate::setupStatusBar(QMainWindow* mainWindow) {
  currentProjectLabel = new QLabel(mainWindow);
  currentProjectLabel->setObjectName("CurrentProjectLabel");
  currentCRSLabel = new QLabel(mainWindow);
  currentCRSLabel->setObjectName("currentCRSLabel");
  progressBar = new QProgressBar(mainWindow);
  progressBar->setObjectName("ProgressBar");

  StatusBar->addWidget(currentProjectLabel);
  StatusBar->addWidget(currentCRSLabel);
  StatusBar->addPermanentWidget(progressBar);
}

void qColadaAppMainWindowPrivate::setupMenuBar(QMainWindow* mainWindow) {
  // Fill in all menus 
  setupFileMenu(mainWindow);
  setupViewMenu(mainWindow);
}

void qColadaAppMainWindowPrivate::setupFileMenu(QMainWindow* mainWindow) {
  Q_Q(qColadaAppMainWindow);

  QList<QAction*> actionList = FileMenu->actions();
  for (int i = 0; i < actionList.count(); i++)
    FileMenu->removeAction(actionList[i]);

  QAction* newProjectAction = new QAction("New project", mainWindow);
  newProjectAction->setObjectName("NewProjectAction");
  QObject::connect(newProjectAction, &QAction::triggered, q, &qColadaAppMainWindow::on_NewProjectAction_triggered);

  QAction* openProjectAction = new QAction("Open project", mainWindow);
  openProjectAction->setObjectName("OpenProjectAction");
  QObject::connect(openProjectAction, &QAction::triggered, q, &qColadaAppMainWindow::on_OpenProjectAction_triggered);

  FileMenu->addActions({ newProjectAction, openProjectAction });
}

void qColadaAppMainWindowPrivate::setupViewMenu(QMainWindow* mainWindow) {
  Q_Q(qColadaAppMainWindow);

  QList<QAction*> actionList = ViewMenu->actions();
  
  ViewMenu->insertActions(actionList[3],
    { seisDockWidget->toggleViewAction(),
    surfDockWidget->toggleViewAction(),
    wellDockWidget->toggleViewAction() });
}

//-----------------------------------------------------------------------------
// qColadaAppMainWindow methods

//-----------------------------------------------------------------------------
qColadaAppMainWindow::qColadaAppMainWindow(QWidget* windowParent)
  : Superclass(new qColadaAppMainWindowPrivate(*this), windowParent)
{
  Q_D(qColadaAppMainWindow);
  d->init();

  dbcore::initProjLibDB();
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

void qColadaAppMainWindow::on_NewProjectAction_triggered()
{
  qColadaNewProject* newproject = new qColadaNewProject();
  newproject->exec();
}

void qColadaAppMainWindow::on_OpenProjectAction_triggered()
{
  Q_D(qColadaAppMainWindow);

  QString projFullName = ctkFileDialog::getOpenFileName(nullptr, QObject::tr("Open Project"), "", QObject::tr("Project file (*.db)"));
  if (projFullName.isEmpty())
    return;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(projFullName);
  if (!db.open())
    QMessageBox::critical(this, "Error", "QSqlDatabase: Can't open project database!");

  d->currentProjectLabel->setText(QSqlDatabase::database().databaseName());
  QString authName, code, CRSname;
  if (!dbcore::getCurrentProjection(authName, code, CRSname))
    QMessageBox::critical(this, "Error",
                          "QSqlDatabase: Can't get projection!");
  d->currentCRSLabel->setText(CRSname + ":" + authName + ":" + code);
}

QDockWidget* qColadaAppMainWindow::getSurfDockWidget() {
  Q_D(qColadaAppMainWindow);
  return d->surfDockWidget;
}

QDockWidget *qColadaAppMainWindow::getSeisDockWidget() {
  Q_D(qColadaAppMainWindow);
  return d->seisDockWidget;
}

QDockWidget *qColadaAppMainWindow::getWellDockWidget() {
  Q_D(qColadaAppMainWindow);
  return d->wellDockWidget;
}

qColadaH5SurfTreeView *qColadaAppMainWindow::getSurfTreeView() {
  Q_D(qColadaAppMainWindow);
  return d->surfTreeView;
}

qColadaH5SeisTreeView *qColadaAppMainWindow::getSeisTreeView() {
  Q_D(qColadaAppMainWindow);
  return d->seisTreeView;
}

qColadaH5WellTreeView *qColadaAppMainWindow::getWellTreeView() {
  Q_D(qColadaAppMainWindow);
  return d->wellTreeView;
}