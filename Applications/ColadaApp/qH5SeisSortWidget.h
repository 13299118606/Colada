#ifndef __qH5SeisSortWidget_h
#define __qH5SeisSortWidget_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class ctkRangeWidget;
class qH5SeisSortWidgetPrivate;
class QComboBox;
class H5Seis;

class Q_COLADA_APP_EXPORT qH5SeisSortWidget : public QWidget {
  Q_OBJECT

public:
  explicit qH5SeisSortWidget(QWidget *parent = nullptr);
  virtual ~qH5SeisSortWidget() override;

public slots:
  void setSeis(H5Seis* seis);
  bool setSeisFromGroupId(ptrdiff_t seisGroupId);
  /// Called each time seis is changed
  void updateWidgets();
  void updateRangeWidget(ctkRangeWidget* rangeWidget);
  void onAddSortBtnClicked(bool val);
  void onRemoveSortBtnClicked(bool val);
  void addKeysModelToComboBox(QComboBox* combo);
  void onSortComboCurrentTextChanged(const QString &text);

  H5Seis* getSeis();
  ptrdiff_t getSeisGroupId();
  QStringList getHeadersLimits(QList<double>& minList, QList<double>& maxList);


protected:
  QScopedPointer<qH5SeisSortWidgetPrivate> d_ptr;

  explicit qH5SeisSortWidget(qH5SeisSortWidgetPrivate *pimpl,
    QWidget *parent);

  QWidget* createWidgetPair();

private:
  Q_DECLARE_PRIVATE(qH5SeisSortWidget);
  Q_DISABLE_COPY(qH5SeisSortWidget);
};

#endif
