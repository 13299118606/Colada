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

#ifndef __qSlicerCaptionsModuleWidget_h
#define __qSlicerCaptionsModuleWidget_h

// Slicer includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerCaptionsModuleExport.h"

class qSlicerCaptionsModuleWidgetPrivate;
class vtkMRMLNode;
class qMRMLThreeDWidget;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_CAPTIONS_EXPORT qSlicerCaptionsModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCaptionsModuleWidget(QWidget *parent=0);
  virtual ~qSlicerCaptionsModuleWidget();
  void addViewCaptionsToolButton(qMRMLThreeDWidget* threeDWidget);

public slots:
  void onThreeDViewCreated(QWidget* view);
  void onViewCaptionsBtnToggled(bool checked);
  void onCaptionsVisibilityToggled(bool state, vtkMRMLNode* node);

protected:
  QScopedPointer<qSlicerCaptionsModuleWidgetPrivate> d_ptr;

  void setup() override;

private:
  Q_DECLARE_PRIVATE(qSlicerCaptionsModuleWidget);
  Q_DISABLE_COPY(qSlicerCaptionsModuleWidget);
};

#endif
