#ifndef __qCRSWidget_p_h
#define __qCRSWidget_p_h

// Colada includes
#include "qCRSWidget.h"

class QTableView;
class QSortFilterProxyModel;
class QStandardItemModel;
class ctkSearchBox;
class QTextBrowser;
class QLineEdit;
class QVBoxLayout;
class QSplitter;

class Q_COLADA_APP_EXPORT qCRSWidgetPrivate {
  Q_DECLARE_PUBLIC(qCRSWidget);

protected:
  qCRSWidget *const q_ptr;

public:
  typedef qCRSWidgetPrivate Superclass;
  qCRSWidgetPrivate(qCRSWidget &q);
  virtual ~qCRSWidgetPrivate();

  virtual void init();
  virtual void setupUi(QWidget *q);

private:
  virtual void setupTableData(QWidget *q);

public:
  QTableView *tableView;
  QSortFilterProxyModel *proxy;
  QStandardItemModel *model;
  ctkSearchBox *searchBox;
  QTextBrowser *txtBrowser;
  QLineEdit *unitsLineEdit;
  QSplitter *splitter;
  QVBoxLayout *mainLayout;
};

#endif