#ifndef __qColadaqColadaGainWidget_h
#define __qColadaqColadaGainWidget_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qColadaGainWidgetPrivate;
class ctkDoubleSlider;
class qScienceSpinBox;

class Q_COLADA_APP_EXPORT qColadaGainWidget : public QWidget {
  Q_OBJECT
public:
  explicit qColadaGainWidget(QWidget *parent = nullptr);
  virtual ~qColadaGainWidget() override;

public slots:
  void setNumberOfSliderSteps(int val);
  void setGain(double val);
  double getGain();
  ctkDoubleSlider *getSlicer();
  qScienceSpinBox *getSpinbox();

private slots:
  void onSliderMoved(double val);
  void onSpinboxEditingFinished();
  void onSliderReleased();

signals:
  void gainChanged(double val);

protected:
  QScopedPointer<qColadaGainWidgetPrivate> d_ptr;

  qColadaGainWidget(qColadaGainWidgetPrivate *pimpl, QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaGainWidget);
  Q_DISABLE_COPY(qColadaGainWidget);
};

#endif // __qColadaqColadaGainWidget_h
