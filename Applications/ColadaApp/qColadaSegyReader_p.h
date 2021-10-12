#ifndef __qColadaSegyReader_p_h
#define __qColadaSegyReader_p_h

// Colada includes
#include "qColadaSegyReader.h"
#include "qColadaReader_p.h"

class QTextBrowser;
class qCopyPasteTableView;

class Q_COLADA_APP_EXPORT qColadaSegyReaderPrivate : public qColadaReaderPrivate {
  Q_DECLARE_PUBLIC(qColadaSegyReader);

public:
  qColadaSegyReaderPrivate(qColadaSegyReader &q);
  virtual ~qColadaSegyReaderPrivate();

  virtual void init() override;
  virtual void initVars() override;
  virtual void initTable() override;
  void initTxtHdrBrowser();
  void initTrcHdrBytesTable();

public:
  QTextBrowser* txtHdrBrowser;
  QSplitter *hSplitter;
  qCopyPasteTableView *trcHdrBytesTableView;
  QSortFilterProxyModel *trcHdrBytesProxy;
  QStandardItemModel *trcHdrBytesModel;
};

#endif
