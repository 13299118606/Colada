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

  This file was originally developed by Johan Andruejol, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qSlicerSpatialReferenceSettingsPanel_h
#define __qSlicerSpatialReferenceSettingsPanel_h

// Qt includes
#include <QString>
#include <QStringList>

// CTK includes
#include <ctkVTKObject.h>
#include <ctkSettingsPanel.h>

// Slicer includes
class vtkMRMLNode;

// Units includes
#include "qSlicerSpatialReferenceModuleExport.h"
class qSlicerSpatialReferenceSettingsPanelPrivate;
class vtkSlicerUnitsLogic;

class Q_SLICER_QTMODULES_SPATIALREFERENCE_EXPORT qSlicerSpatialReferenceSettingsPanel
  : public ctkSettingsPanel
{
  Q_OBJECT
  QVTK_OBJECT
public:

  /// Superclass typedef
  typedef ctkSettingsPanel Superclass;

  /// Constructor
  qSlicerSpatialReferenceSettingsPanel(QWidget* parent = nullptr);

  /// Destructor
  ~qSlicerSpatialReferenceSettingsPanel() override;

protected slots:
  void onTimeUnitWidgetSuffixChanged(QString name);
  void onLengthUnitWidgetSuffixChanged(QString name);
  void onCRSChanged(
      const QString& name,
      const QString& authName,
      const QString& code);

protected:
  QScopedPointer<qSlicerSpatialReferenceSettingsPanelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerSpatialReferenceSettingsPanel);
  Q_DISABLE_COPY(qSlicerSpatialReferenceSettingsPanel);
};

#endif
