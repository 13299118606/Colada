#ifndef __qColadaH5Item_p_h
#define __qColadaH5Item_p_h

// Qt includes
#include <QVector>

// h5geo includes
#include <h5geo/h5basecontainer.h>
#include <h5geo/h5baseobject.h>

// Colada includes
#include "qColadaH5Item.h"

class Q_COLADA_APP_EXPORT qColadaH5ItemPrivate {
  Q_DECLARE_PUBLIC(qColadaH5Item);

protected:
  qColadaH5Item *const q_ptr;

public:
  typedef qColadaH5ItemPrivate Superclass;
  qColadaH5ItemPrivate(qColadaH5Item &q);
  virtual ~qColadaH5ItemPrivate();

protected:
  QVector<qColadaH5Item *> childItems;
  H5Base_ptr itemData = nullptr;
  qColadaH5Item *parentItem = nullptr;
  bool mapped = false;
  Qt::CheckState checkState = Qt::Unchecked;
};

#endif
