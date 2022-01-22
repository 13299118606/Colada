#ifndef __qH5SeisSortWidget_p_h
#define __qH5SeisSortWidget_p_h

#include "qH5SeisSortWidget.h"

#include <memory>

class QHBoxLayout;
class QVBoxLayout;
class QToolButton;
class qCompletableComboBox;
class ctkRangeWidget;
class H5Seis;
namespace h5geo {
struct ObjectDeleter;
}

class Q_COLADA_APP_EXPORT qH5SeisSortWidgetPrivate {
  Q_DECLARE_PUBLIC(qH5SeisSortWidget);

protected:
  qH5SeisSortWidget *const q_ptr;

public:
  typedef qH5SeisSortWidgetPrivate Superclass;
  qH5SeisSortWidgetPrivate(qH5SeisSortWidget &q);
  virtual ~qH5SeisSortWidgetPrivate();

  virtual void init();

  QHBoxLayout* addRemoveSortHLayout;
  QVBoxLayout* mainVLayout;
  qCompletableComboBox* pKeyComboBox;
  ctkRangeWidget* pKeyRangeWidget;
  QToolButton* addSortBtn, *removeSortBtn;
//  H5Seis* seis;
  std::unique_ptr<H5Seis, h5geo::ObjectDeleter> seis;
};

#endif
