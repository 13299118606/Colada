#ifndef __qSlicerCaptionsWidget_h
#define __qSlicerCaptionsWidget_h

// CTK includes
#include <ctkVTKObject.h>

// Slicer includes
#include <qSlicerWidget.h>

#include "qSlicerCaptionsModuleWidgetsExport.h"

class qSlicerCaptionsWidgetPrivate;
class vtkMRMLNode;
class vtkObject;
class qMRMLNodeComboBox;

/// \ingroup Slicer_QtModules_Captions
class Q_SLICER_MODULE_CAPTIONS_WIDGETS_EXPORT qSlicerCaptionsWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
public:
  typedef qSlicerWidget Superclass;
  qSlicerCaptionsWidget(QWidget *parent=0);
  virtual ~qSlicerCaptionsWidget();

  void setCaptionsBtnToggled(bool val);

protected:
  void setupCaptionsFromWidget();

public slots:
  void onCheckBoxStateChanged(int state);
  void onColorChanged(QColor c);
  void onTextChanged(const QString& text);
  void onSpinBoxValueChanged(int val);
  void onDoubleSpinBoxValueChanged(double val);
  void onComboBoxIndexChanged(int val);
  void onCaptionsBtnToggled(bool val);

signals:
  void captionsVisibilityToggled(bool state, vtkMRMLNode* viewNode);

protected:
  QScopedPointer<qSlicerCaptionsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerCaptionsWidget);
  Q_DISABLE_COPY(qSlicerCaptionsWidget);
};

#endif
