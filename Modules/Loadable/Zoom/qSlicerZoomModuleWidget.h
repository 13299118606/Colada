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
class qMRMLSliceWidget;
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
  void addFullFitToWindowToolButton(qMRMLSliceWidget* sliceWidget);

public slots:
  // handle newly created qMRMLSliceWidget to add tool btn
  void onSliceViewCreated(QWidget* view);
  void onFullFitToWindowBtnClicked();

protected:
  QScopedPointer<qSlicerZoomModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerZoomModuleWidget);
  Q_DISABLE_COPY(qSlicerZoomModuleWidget);
};

#endif
