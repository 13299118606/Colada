#ifndef __qH5SeisSortWidget_p_h
#define __qH5SeisSortWidget_p_h

#include "qH5SeisSortWidget.h"

#include <memory>

class QHBoxLayout;
class QVBoxLayout;
class QToolButton;
class qCompletableComboBox;
class ctkRangeWidget;
class qH5ItemDropLineEdit;

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
  std::vector<std::string> pKeyNames;
  std::map<std::string, double> traceHeadersMin, traceHeadersMax;
};

#endif
