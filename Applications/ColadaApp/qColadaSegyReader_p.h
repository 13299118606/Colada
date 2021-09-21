#ifndef __qColadaSegyReader_p_h
#define __qColadaSegyReader_p_h

// Colada includes
#include "qColadaSegyReader.h"
#include "qColadaReader_p.h"

class QTextBrowser;

class Q_COLADA_APP_EXPORT qColadaSegyReaderPrivate : public qColadaReaderPrivate {
  Q_DECLARE_PUBLIC(qColadaSegyReader);

public:
  qColadaSegyReaderPrivate(qColadaSegyReader &q);
  virtual ~qColadaSegyReaderPrivate();

  virtual void init();
  virtual void initVars() override;
  virtual void initTable() override;
  void initTxtHdrBrowser();

public:
  QTextBrowser* txtHdrBrowser;
  QSplitter *hSplitter;
};

#endif