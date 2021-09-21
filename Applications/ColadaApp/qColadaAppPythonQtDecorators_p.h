#ifndef __qColadaH5Model_TEST_p_h
#define __qColadaH5Model_TEST_p_h

// Colada includes
#include "qColadaH5Model_TEST.h"

class Q_COLADA_APP_EXPORT qColadaH5Model_TESTPrivate
{
	Q_DECLARE_PUBLIC(qColadaH5Model_TEST);

protected:
	qColadaH5Model_TEST* const q_ptr;

public:
	typedef qColadaH5Model_TESTPrivate Superclass;
	qColadaH5Model_TESTPrivate(qColadaH5Model_TEST& q);
	virtual ~qColadaH5Model_TESTPrivate();

	virtual void init(QString title);

// protected:
// 	qColadaH5Item* rootItem;
//   QString headerData;
};

#endif