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

#ifndef __qSlicerSceneFilterWidget_h
#define __qSlicerSceneFilterWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include <qSlicerWidget.h>

// Stack3D Widgets includes
#include "qSlicerSceneFilterModuleWidgetsExport.h"

class qSlicerSceneFilterWidgetPrivate;

/// \ingroup Slicer_QtModules_Axes
class Q_SLICER_MODULE_SCENEFILTER_WIDGETS_EXPORT qSlicerSceneFilterWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerSceneFilterWidget(QWidget *parent=0);
  virtual ~qSlicerSceneFilterWidget();

public slots:
  void onLineEditEditingFinished();
  void onDomainComboBoxCurrentTextChanged(const QString& text);

signals:

protected:
  QScopedPointer<qSlicerSceneFilterWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerSceneFilterWidget);
  Q_DISABLE_COPY(qSlicerSceneFilterWidget);
};

#endif
