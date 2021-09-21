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

#ifndef __qSlicerZoomFooBarWidget_h
#define __qSlicerZoomFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "qSlicerZoomModuleWidgetsExport.h"

class qSlicerZoomFooBarWidgetPrivate;

/// \ingroup Slicer_QtModules_Zoom
class Q_SLICER_MODULE_ZOOM_WIDGETS_EXPORT qSlicerZoomFooBarWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerZoomFooBarWidget(QWidget *parent=0);
  virtual ~qSlicerZoomFooBarWidget();

protected slots:

protected:
  QScopedPointer<qSlicerZoomFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerZoomFooBarWidget);
  Q_DISABLE_COPY(qSlicerZoomFooBarWidget);
};

#endif
