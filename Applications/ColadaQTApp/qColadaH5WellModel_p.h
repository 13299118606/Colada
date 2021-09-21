#ifndef __qColadaH5WellModel_p_h
#define __qColadaH5WellModel_p_h

// Colada includes
#include "qColadaH5Model_p.h"
#include "qColadaH5WellModel.h"

class Q_COLADA_APP_EXPORT qColadaH5WellModelPrivate
    : public qColadaH5ModelPrivate {
  Q_DECLARE_PUBLIC(qColadaH5WellModel);

public:
  typedef qColadaH5ModelPrivate Superclass;
  qColadaH5WellModelPrivate(qColadaH5WellModel &q);
  virtual ~qColadaH5WellModelPrivate();

  virtual void init(QString title) override;
};

#endif
