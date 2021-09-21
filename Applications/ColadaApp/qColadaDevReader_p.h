#ifndef __qColadaDevReader_p_h
#define __qColadaDevReader_p_h

// Colada includes
#include "qColadaReader_p.h"
#include "qColadaDevReader.h"

class Q_COLADA_APP_EXPORT qColadaDevReaderPrivate
    : public qColadaReaderPrivate {
  Q_DECLARE_PUBLIC(qColadaDevReader);

public:
  qColadaDevReaderPrivate(qColadaDevReader &q);
  virtual ~qColadaDevReaderPrivate();

  virtual void init();
  virtual void initVars() override;
  virtual void initTable() override;
};

#endif