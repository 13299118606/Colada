#ifndef __qREValidator_p_h
#define __qREValidator_p_h

// Colada includes
#include "qREValidator.h"

class QString;

class Q_COLADA_APP_EXPORT qREValidatorPrivate
{
	Q_DECLARE_PUBLIC(qREValidator);

protected:
	qREValidator* const q_ptr;

public:
  typedef qREValidatorPrivate Superclass;
	qREValidatorPrivate(qREValidator& object);
  virtual ~qREValidatorPrivate();

	virtual void init(const QString &toolTipText, int toolTipDuration);

private:
	QString toolTipText;
	int toolTipDuration = 3000;
};

#endif