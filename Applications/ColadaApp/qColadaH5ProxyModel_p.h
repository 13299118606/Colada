#ifndef __qColadaH5ProxyModel_p_h
#define __qColadaH5ProxyModel_p_h

// Colada includes
#include "qColadaH5ProxyModel.h"

class Q_COLADA_APP_EXPORT qColadaH5ProxyModelPrivate {
  Q_DECLARE_PUBLIC(qColadaH5ProxyModel);

protected:
  qColadaH5ProxyModel *const q_ptr;

public:
  typedef qColadaH5ProxyModelPrivate Superclass;
  qColadaH5ProxyModelPrivate(qColadaH5ProxyModel &q);
  virtual ~qColadaH5ProxyModelPrivate();

private:
  bool showCheckedOnlyFlag = false;
  bool showHardLinks = true;
  bool showSoftLinks = true;
  bool showExternalLinks = true;
};

#endif
