#ifndef __qColadaH5MapModel_p_h
#define __qColadaH5MapModel_p_h

// Colada includes
#include "qColadaH5MapModel.h"
#include "qColadaH5Model_p.h"

class Q_COLADA_APP_EXPORT qColadaH5MapModelPrivate
    : public qColadaH5ModelPrivate 
{
  Q_DECLARE_PUBLIC(qColadaH5MapModel);

public:
  typedef qColadaH5ModelPrivate Superclass;
  qColadaH5MapModelPrivate(qColadaH5MapModel &q);
  virtual ~qColadaH5MapModelPrivate();

  virtual void init(const QString &title) override;
};

#endif
