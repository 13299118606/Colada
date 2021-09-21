#ifndef __qColadaH5Model_p_h
#define __qColadaH5Model_p_h

// Colada includes
#include "qColadaH5Model.h"

class Q_COLADA_APP_EXPORT qColadaH5ModelPrivate
{
	Q_DECLARE_PUBLIC(qColadaH5Model);

protected:
	qColadaH5Model* const q_ptr;

public:
	typedef qColadaH5ModelPrivate Superclass;
	qColadaH5ModelPrivate(qColadaH5Model& q);
	virtual ~qColadaH5ModelPrivate();

	virtual void init(const QString &title);

protected:
	qColadaH5Item* rootItem;
  QString headerData;
};

#endif