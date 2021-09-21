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

#ifndef __qSlicerZoomModuleWidget_h
#define __qSlicerZoomModuleWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerZoomModuleExport.h"

class qSlicerZoomModuleWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLViewNode;
class vtkMRMLSliceNode;
class vtkSlicerZoomLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_ZOOM_EXPORT qSlicerZoomModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerZoomModuleWidget(QWidget *parent=0);
  virtual ~qSlicerZoomModuleWidget();

protected:
  void setupWidgetSettingsFromSliceNode(vtkMRMLSliceNode* sliceNode);
  void setupWidgetSettingsFromView3DNode(vtkMRMLViewNode* viewNode);
  bool eventFilter(QObject *object, QEvent *event);

public slots:
  void onCurrentViewNode2DChanged(vtkMRMLNode* node);
  void onCurrentViewNode3DChanged(vtkMRMLNode* node);
  void on2DSliceGainChanged(double oldGain, double newGain);
  void on3DViewGainChanged(double gain);
  void onView3DNodeModified(vtkObject* sender);
  void onSliceNodeModified(vtkObject* sender);
  void onSliceViewResized(const QSize& newSize);

protected:
  QScopedPointer<qSlicerZoomModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerZoomModuleWidget);
  Q_DISABLE_COPY(qSlicerZoomModuleWidget);
};

#endif
