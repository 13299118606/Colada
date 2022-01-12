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

#ifndef __qSlicerZoom2DWidget_h
#define __qSlicerZoom2DWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include <qSlicerWidget.h>

// 2D Widgets includes
#include "qSlicerZoomModuleWidgetsExport.h"

class qSlicerZoom2DWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLSliceNode;

/// \ingroup Slicer_QtModules_Zoom
class Q_SLICER_MODULE_ZOOM_WIDGETS_EXPORT qSlicerZoom2DWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerZoom2DWidget(QWidget *parent=0);
  virtual ~qSlicerZoom2DWidget();

protected:
  void setupWidgetFromZoom();
  void setupZoomFromWidget();

public slots:
  void onCurrentSliceNodeChanged(vtkMRMLNode* node);
  void onSliceGainChanged(double oldGain, double newGain);
  void onSliceNodeModified(vtkObject* sender);

protected:
  QScopedPointer<qSlicerZoom2DWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerZoom2DWidget);
  Q_DISABLE_COPY(qSlicerZoom2DWidget);
};

#endif
