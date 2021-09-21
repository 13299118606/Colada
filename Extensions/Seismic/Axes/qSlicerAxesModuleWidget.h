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

#ifndef __qSlicerAxesModuleWidget_h
#define __qSlicerAxesModuleWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerAxesModuleExport.h"

class qSlicerAxesModuleWidgetPrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_AXES_EXPORT qSlicerAxesModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerAxesModuleWidget(QWidget *parent=0);
  virtual ~qSlicerAxesModuleWidget();

protected:
  virtual void setup();
  void setupMRMLAxesDisplayableManager2D();
  void setupMRMLAxesDisplayableManager3D();
  void setupWidgetSettingsFromMRMLAxesDisplayableManager2D();
  void setupWidgetSettingsFromMRMLAxesDisplayableManager3D();
  void updateAxes2DSettings();
  void updateAxes3DSettings();

public slots:
  void onCurrentViewNode2DChanged(vtkMRMLNode* node);
  void onCurrentViewNode3DChanged(vtkMRMLNode* node);
  void onUpdateAxes2DBtnclicked();
  void onUpdateAxes3DBtnclicked();
  void onShowAxes2DBtnToggled(bool val);
  void onShowAxes3DBtnToggled(bool val);
  void onCubeAxesActorModified(vtkObject* sender);

protected:
  QScopedPointer<qSlicerAxesModuleWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerAxesModuleWidget);
  Q_DISABLE_COPY(qSlicerAxesModuleWidget);
};

#endif
