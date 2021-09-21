#ifndef __qSlicerqSlicerGainWidget_h
#define __qSlicerqSlicerGainWidget_h

// Qt includes
#include <QWidget>

// Slicer includes
#include "qSlicerZoomModuleWidgetsExport.h"

class qSlicerGainWidgetPrivate;
class ctkDoubleSlider;
class qScienceSpinBox;

class Q_SLICER_MODULE_ZOOM_WIDGETS_EXPORT qSlicerGainWidget : public QWidget {
  Q_OBJECT
public:
  explicit qSlicerGainWidget(QWidget *parent = nullptr);
  virtual ~qSlicerGainWidget() override;

public slots:
  void setNumberOfSliderSteps(int val);
  void setGain(double val);
  double getGain();
  ctkDoubleSlider *getSlider();
  qScienceSpinBox *getSpinbox();
  void setMinimum(double val);
  void setMaximum(double val);

private slots:
  void onSliderMoved(double val);
  void onSpinboxEditingFinished();
  void onSliderReleased();

signals:
  void gainChanged(double val);
  void gainChanged(double oldVal, double newVal);

protected:
  QScopedPointer<qSlicerGainWidgetPrivate> d_ptr;

  qSlicerGainWidget(qSlicerGainWidgetPrivate *pimpl, QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qSlicerGainWidget);
  Q_DISABLE_COPY(qSlicerGainWidget);
};

#endif // __qSlicerqSlicerGainWidget_h
