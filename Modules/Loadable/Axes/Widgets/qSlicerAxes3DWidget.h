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

#ifndef __qSlicerAxes3DWidget_h
#define __qSlicerAxes3DWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include <qSlicerWidget.h>

// Stack3D Widgets includes
#include "qSlicerAxesModuleWidgetsExport.h"

class qSlicerAxes3DWidgetPrivate;
class vtkMRMLNode;
class vtkObject;
class qMRMLNodeComboBox;

/// \ingroup Slicer_QtModules_Axes
class Q_SLICER_MODULE_AXES_WIDGETS_EXPORT qSlicerAxes3DWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerAxes3DWidget(QWidget *parent=0);
  virtual ~qSlicerAxes3DWidget();

protected:
  void setupWidgetFromAxes();
  void setupAxesFromWidget();

public slots:
  void onCurrentNodeChanged(vtkMRMLNode* node);
  void onCurrentViewNodeChanged(vtkMRMLNode* node);
  void onCheckBoxStateChanged(int state);
  void onColorChanged(QColor c);
  void onTextChanged(const QString& text);
  void onSpinBoxValueChanged(int val);
  void onDoubleSpinBoxValueChanged(double val);
  void onComboBoxIndexChanged(int val);
  void onAxesBtnToggled(bool val);
  void onCubeAxesActorModified(vtkObject* sender);

signals:
  void axes3DVisibilityToggled(bool state, vtkMRMLNode* viewNode);

protected:
  QScopedPointer<qSlicerAxes3DWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerAxes3DWidget);
  Q_DISABLE_COPY(qSlicerAxes3DWidget);
};

#endif
