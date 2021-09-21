#ifndef __REValidator_h
#define __REValidator_h

// Qt includes
#include <QRegularExpressionValidator>

// Colada includes
#include "qColadaAppExport.h"

class qREValidatorPrivate;

class Q_COLADA_APP_EXPORT qREValidator
	: public QRegularExpressionValidator
{
	Q_OBJECT

public:
  explicit qREValidator(QObject *parent = nullptr);

  explicit qREValidator(const QString &toolTipText, int toolTipDuration = 3000,
                        QObject *parent = nullptr);

  explicit qREValidator(const QRegularExpression &re,
                        const QString &toolTipText, 
                        int toolTipDuration = 3000, QObject *parent = nullptr);

  virtual ~qREValidator() override;

public slots:
	virtual QValidator::State validate(
		QString& input, int& pos) const;

	void setToolTipText(const QString &str);
	void setToolTipDuration(int val);
	QString getToolTipText();
	int getToolTipDuration();

protected:
	QScopedPointer<qREValidatorPrivate> d_ptr;

  explicit qREValidator(qREValidatorPrivate *pimpl, QObject *parent);

private:
	Q_DECLARE_PRIVATE(qREValidator);
	Q_DISABLE_COPY(qREValidator);
};

#endif
