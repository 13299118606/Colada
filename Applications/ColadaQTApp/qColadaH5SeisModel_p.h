#ifndef __qColadaH5SeisModel_p_h
#define __qColadaH5SeisModel_p_h

// Colada includes
#include "qColadaH5Model_p.h"
#include "qColadaH5SeisModel.h"

class Q_COLADA_APP_EXPORT qColadaH5SeisModelPrivate
    : public qColadaH5ModelPrivate {
  Q_DECLARE_PUBLIC(qColadaH5SeisModel);

public:
  typedef qColadaH5ModelPrivate Superclass;
  qColadaH5SeisModelPrivate(qColadaH5SeisModel &q);
  virtual ~qColadaH5SeisModelPrivate();

  virtual void init(QString title) override;
};

#endif
