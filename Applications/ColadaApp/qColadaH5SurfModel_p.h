#ifndef __qColadaH5SurfModel_p_h
#define __qColadaH5SurfModel_p_h

// Colada includes
#include "qColadaH5SurfModel.h"
#include "qColadaH5Model_p.h"

class Q_COLADA_APP_EXPORT qColadaH5SurfModelPrivate
    : public qColadaH5ModelPrivate 
{
  Q_DECLARE_PUBLIC(qColadaH5SurfModel);

public:
  typedef qColadaH5ModelPrivate Superclass;
  qColadaH5SurfModelPrivate(qColadaH5SurfModel &q);
  virtual ~qColadaH5SurfModelPrivate();

  virtual void init(const QString &title) override;
};

#endif
