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
  setupSliceNodes(mainWindow);
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

void qColadaAppMainWindowPrivate::setupSliceNodes(QMainWindow * mainWindow)
{
  Q_Q(qColadaAppMainWindow);
  std::vector<std::string> axesNames({"-X", "X", "-Y", "Y", "-Z", "Z"});
  std::vector<std::string> orientationPresetOld({"Axial", "Sagittal", "Coronal"});
  std::vector<std::string> orientationPresetNew({"XY", "YZ", "XZ"});
  std::vector<std::string> defaultOrientations({"XY", "YZ", "XZ"});

  // Default View nodes
  vtkSmartPointer<vtkMRMLNode> dViewNode =
      this->LayoutManager->mrmlScene()->GetDefaultNodeByClass("vtkMRMLViewNode");
  if (!dViewNode)
  {
    vtkMRMLNode *node = this->LayoutManager->mrmlScene()->CreateNodeByClass("vtkMRMLViewNode");
    dViewNode.TakeReference(node);
    this->LayoutManager->mrmlScene()->AddDefaultNode(dViewNode);
  }
  vtkMRMLViewNode *defaultViewNode = vtkMRMLViewNode::SafeDownCast(dViewNode);
  defaultViewNode->DisableModifiedEventOn();

  for (size_t i = 0; i < axesNames.size(); i++)
    defaultViewNode->SetAxisLabel(
          i, axesNames[i].c_str());

  // Default Slice nodes
  vtkSmartPointer<vtkMRMLNode> dSliceNode =
      this->LayoutManager->mrmlScene()->GetDefaultNodeByClass("vtkMRMLSliceNode");
  if (!dSliceNode)
  {
    vtkMRMLNode *node = this->LayoutManager->mrmlScene()->CreateNodeByClass("vtkMRMLSliceNode");
    dSliceNode.TakeReference(node);
    this->LayoutManager->mrmlScene()->AddDefaultNode(dSliceNode);
  }
  vtkMRMLSliceNode *defaultSliceNode = vtkMRMLSliceNode::SafeDownCast(dSliceNode);
  defaultSliceNode->DisableModifiedEventOn();

  for (size_t i = 0; i < axesNames.size(); i++)
    defaultSliceNode->SetAxisLabel(
          i, axesNames[i].c_str());

  defaultSliceNode->SetDefaultOrientation(defaultOrientations[0].c_str());
  for (size_t i = 0; i < orientationPresetOld.size(); i++){
    if (defaultSliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
      defaultSliceNode->RemoveSliceOrientationPreset(
            orientationPresetOld[i]);
    }

    if (!defaultSliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
      defaultSliceNode->AddSliceOrientationPreset(
            orientationPresetNew[i],
            GenerateOrientationMatrix(orientationPresetNew[i]));
    }
  }
  defaultSliceNode->DisableModifiedEventOff();

  // View nodes
  for (int j = 0; j < this->LayoutManager->threeDViewCount(); j++)
    for (int i = 0; i < axesNames.size(); i++)
      this->LayoutManager->threeDWidget(j)->mrmlViewNode()->SetAxisLabel(
          i, axesNames[i].c_str());

  // Slice nodes
  size_t ii = 0;
  for (const QString& sliceViewName : this->LayoutManager->sliceViewNames()){
    vtkMRMLSliceNode *sliceNode = this->LayoutManager->
        sliceWidget(sliceViewName)->mrmlSliceNode();

    for (int i = 0; i < axesNames.size(); i++)
      sliceNode->SetAxisLabel(i, axesNames[i].c_str());

    for (size_t i = 0; i < orientationPresetOld.size(); i++){
      if (sliceNode->HasSliceOrientationPreset(orientationPresetOld[i])){
        sliceNode->RemoveSliceOrientationPreset(
              orientationPresetOld[i]);
      }

      if (!sliceNode->HasSliceOrientationPreset(orientationPresetNew[i])){
        sliceNode->AddSliceOrientationPreset(
              orientationPresetNew[i],
              GenerateOrientationMatrix(orientationPresetNew[i]));
      }
    }

    if (ii < defaultOrientations.size())
      sliceNode->SetDefaultOrientation(defaultOrientations[ii].c_str());
    else
      sliceNode->SetDefaultOrientation(defaultOrientations[0].c_str());
    ii++;
  }

  // Replace layout description with new presets
  vtkMRMLLayoutNode* layoutNode =  vtkMRMLLayoutNode::SafeDownCast(
    this->LayoutManager->mrmlScene()->GetSingletonNode("vtkMRMLLayoutNode","vtkMRMLLayoutNode"));
  if(!layoutNode){
    qCritical() << "qColadaAppMainWindowPrivate::setupSliceNodes: layoutNode not found!";
    return;
  }

  for(int i = 1 ; i < 36; i++)
  {
    if (i == 5 || i == 11 || i == 13 || i == 18 || i == 20)
    {
      continue;
    }

    std::string layoutDescription = layoutNode->GetLayoutDescription(i);
    std::vector<std::string>::const_iterator it;
    std::vector<std::string>::const_iterator jt;
    for(it = orientationPresetOld.begin(), jt = orientationPresetNew.begin();
        it != orientationPresetOld.end() && jt != orientationPresetNew.end(); ++it, ++jt)
    {
      size_t found = layoutDescription.find(*it);
      while (found!=std::string::npos)
      {
        layoutDescription.replace(found, it->size(), *jt);
        found = layoutDescription.find(*it);
      }
    }
    layoutNode->SetLayoutDescription(i, layoutDescription.c_str());
  }
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

vtkSmartPointer<vtkMatrix3x3> qColadaAppMainWindowPrivate::GenerateOrientationMatrix(const std::string& name)
{
  if (name.compare("XY") == 0 || name.compare("YX") == 0)
    return GenerateXYOrientationMatrix();
  else if (name.compare("XZ") == 0 || name.compare("ZX") == 0)
    return GenerateXZOrientationMatrix();
  else if (name.compare("YZ") == 0 || name.compare("ZY") == 0)
    return GenerateYZOrientationMatrix();
  else
    return vtkSmartPointer<vtkMatrix3x3>::New();
}

vtkSmartPointer<vtkMatrix3x3> qColadaAppMainWindowPrivate::GenerateXYOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  return mtx;
}

vtkSmartPointer<vtkMatrix3x3> qColadaAppMainWindowPrivate::GenerateXZOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  mtx->SetElement(0,0,1);
  mtx->SetElement(1,1,0);
  mtx->SetElement(2,2,0);
  mtx->SetElement(1,2,-1);
  mtx->SetElement(2,1,1);
  return mtx;
}

vtkSmartPointer<vtkMatrix3x3> qColadaAppMainWindowPrivate::GenerateYZOrientationMatrix()
{
  vtkSmartPointer<vtkMatrix3x3> mtx = vtkSmartPointer<vtkMatrix3x3>::New();
  mtx->SetElement(0,0,0);
  mtx->SetElement(1,1,0);
  mtx->SetElement(2,2,0);
  mtx->SetElement(0,2,-1);
  mtx->SetElement(1,0,1);
  mtx->SetElement(2,1,1);
  return mtx;
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
