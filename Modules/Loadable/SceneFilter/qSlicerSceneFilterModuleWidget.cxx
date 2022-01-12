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

// module includes
#include "vtkSlicerSceneFilterLogic.h"

// Qt includes
#include <QDebug>
#include <QStatusBar>
#include <QToolButton>

// Slicer includes
#include "qSlicerSceneFilterModuleWidget.h"
#include "ui_qSlicerSceneFilterModuleWidget.h"
#include "qSlicerSceneFilterWidget.h"
#include "qSlicerApplication.h"
#include "qSlicerMainWindow.h"

// MRML includes
#include "vtkMRMLScene.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerSceneFilterModuleWidgetPrivate: public Ui_qSlicerSceneFilterModuleWidget
{
public:
  qSlicerSceneFilterModuleWidgetPrivate();
  qSlicerApplication* app = nullptr;
  qSlicerMainWindow* mainwindow = nullptr;
  qSlicerSceneFilterWidget* sceneFilterWidget;
};

//-----------------------------------------------------------------------------
// qSlicerSceneFilterModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerSceneFilterModuleWidgetPrivate::qSlicerSceneFilterModuleWidgetPrivate()
{
  app = qSlicerApplication::application();
  if (!app){
    qCritical() << "qSlicerSceneFilterModuleWidgetPrivate::qSlicerSceneFilterModuleWidgetPrivate: " <<
                   "unable to get application instance";
    return;
  }
  mainwindow = qobject_cast<qSlicerMainWindow*>(app->mainWindow());
  if (!mainwindow){
    qCritical() << "qSlicerSceneFilterModuleWidgetPrivate::qSlicerSceneFilterModuleWidgetPrivate: " <<
                   "unable to get mainwindow";
    return;
  }

  if (!mainwindow->statusBar()){
    qCritical() << "qSlicerSceneFilterModuleWidgetPrivate::qSlicerSceneFilterModuleWidgetPrivate: " <<
                   "unable to get status bar";
    return;
  }

  sceneFilterWidget = new qSlicerSceneFilterWidget(mainwindow->statusBar());
  sceneFilterWidget->setObjectName("SceneFilterWidget");

  mainwindow->statusBar()->addPermanentWidget(sceneFilterWidget);

  // set errorToolBtn to the end of status bar
  QToolButton* errorToolBtn = mainwindow->statusBar()->findChild<QToolButton*>();
  if (errorToolBtn)
    mainwindow->statusBar()->addPermanentWidget(errorToolBtn);
}

//-----------------------------------------------------------------------------
// qSlicerSceneFilterModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerSceneFilterModuleWidget::qSlicerSceneFilterModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerSceneFilterModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerSceneFilterModuleWidget::~qSlicerSceneFilterModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerSceneFilterModuleWidget::setup()
{
  Q_D(qSlicerSceneFilterModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(this, &qSlicerSceneFilterModuleWidget::mrmlSceneChanged,
          d->sceneFilterWidget, &qSlicerSceneFilterModuleWidget::setMRMLScene);
  connect(this, &qSlicerSceneFilterModuleWidget::mrmlSceneChanged,
          d->sceneFilterWidget, &qSlicerSceneFilterModuleWidget::setMRMLScene);
}
