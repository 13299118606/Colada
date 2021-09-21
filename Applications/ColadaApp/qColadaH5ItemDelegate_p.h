#ifndef __qColadaH5ItemDelegate_p_h
#define __qColadaH5ItemDelegate_p_h

// Colada includes
#include "qColadaH5ItemDelegate.h"

class Q_COLADA_APP_EXPORT qColadaH5ItemDelegatePrivate
{
	Q_DECLARE_PUBLIC(qColadaH5ItemDelegate);

protected:
	qColadaH5ItemDelegate* const q_ptr;

public:
	typedef qColadaH5ItemDelegatePrivate Superclass;
	qColadaH5ItemDelegatePrivate(qColadaH5ItemDelegate& q);
	virtual ~qColadaH5ItemDelegatePrivate();
};

#endif