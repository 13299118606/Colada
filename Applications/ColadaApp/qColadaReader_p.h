#ifndef __qColadaReader_p_h
#define __qColadaReader_p_h

// Colada includes
#include "qColadaReader.h"

class qCRSDropTableView;
class qCRSWidget;
class ctkCheckableHeaderView;
class QSortFilterProxyModel;
class QStandardItemModel;
class QToolButton;
class QDialogButtonBox;
class QSplitter;
class qSlicerApplication;

class Q_COLADA_APP_EXPORT qColadaReaderPrivate {
  Q_DECLARE_PUBLIC(qColadaReader);

protected:
  qColadaReader *const q_ptr;

public:
  typedef qColadaReaderPrivate Superclass;
  qColadaReaderPrivate(qColadaReader &q);
  virtual ~qColadaReaderPrivate();

  virtual void init();
  virtual void initVars(); // inheritors must implement
  virtual void initTable(); // inheritors must implement
  virtual void setupUi(QDialog *q);

public:
  qSlicerApplication* app;
  qCRSDropTableView *tableView;
  qCRSWidget *crsWidget;
  ctkCheckableHeaderView *hrzHeader, *vertHeader;
  QSortFilterProxyModel *proxy;
  QStandardItemModel *model;
  QToolButton *addToolBtn, *removeToolBtn, *autoDefineBtn;
  QDialogButtonBox *buttonBox;
  QSplitter *mainSplitter, *vSplitter;

  QStringList tableHdrNames, tableHdrTips, spinboxColNameList,
  scSpinboxColNameList, lineEditColNameList, pathEditColNameList;
};

#endif
