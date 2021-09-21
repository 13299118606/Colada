#ifndef __qCopyPasteTableView_p_h
#define __qCopyPasteTableView_p_h

// Colada includes
#include "qCopyPasteTableView.h"

class Q_COLADA_APP_EXPORT qCopyPasteTableViewPrivate {
  Q_DECLARE_PUBLIC(qCopyPasteTableView);

protected:
  qCopyPasteTableView *const q_ptr;

public:
  typedef qCopyPasteTableViewPrivate Superclass;
  qCopyPasteTableViewPrivate(qCopyPasteTableView &q);
  virtual ~qCopyPasteTableViewPrivate();

  virtual void init();
};

#endif