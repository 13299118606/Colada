#ifndef __qCRSDropTableView_p_h
#define __qCRSDropTableView_p_h

// Colada includes
#include "qCRSDropTableView.h"
#include "qCopyPasteTableView_p.h"

class Q_COLADA_APP_EXPORT qCRSDropTableViewPrivate : public qCopyPasteTableViewPrivate {
  Q_DECLARE_PUBLIC(qCRSDropTableView);

public:
  typedef qCopyPasteTableViewPrivate Superclass;
  qCRSDropTableViewPrivate(qCRSDropTableView &q);
  virtual ~qCRSDropTableViewPrivate();

  virtual void init();
};

#endif