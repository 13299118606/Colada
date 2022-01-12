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

#ifndef __qSlicerZoom3DWidget_h
#define __qSlicerZoom3DWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include <qSlicerWidget.h>

// 3D Widgets includes
#include "qSlicerZoomModuleWidgetsExport.h"

class qSlicerZoom3DWidgetPrivate;
class vtkMRMLNode;
class vtkMRMLViewNode;

/// \ingroup Slicer_QtModules_Zoom
class Q_SLICER_MODULE_ZOOM_WIDGETS_EXPORT qSlicerZoom3DWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerZoom3DWidget(QWidget *parent=0);
  virtual ~qSlicerZoom3DWidget();

protected:
  void setupWidgetFromZoom();
  void setupZoomFromWidget();

public slots:
  void onCurrentViewNodeChanged(vtkMRMLNode* node);
  void onViewGainChanged(double gain);
  void onViewNodeModified(vtkObject* sender);

protected:
  QScopedPointer<qSlicerZoom3DWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerZoom3DWidget);
  Q_DISABLE_COPY(qSlicerZoom3DWidget);
};

#endif
