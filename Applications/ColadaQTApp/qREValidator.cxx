// Colada includes
#include "qREValidator.h"
#include "qREValidator_p.h"

// Qt includes
#include <QToolTip>

qREValidatorPrivate::qREValidatorPrivate(qREValidator& q)
	: q_ptr(&q)
{

}

qREValidatorPrivate::~qREValidatorPrivate() {

}

void qREValidatorPrivate::init() {
	Q_Q(qREValidator);
	this->setupUi(q);
}

void qREValidatorPrivate::setupUi(QRegularExpressionValidator* q) {

}

qREValidator::qREValidator(
	int toolTipDuration, QObject* parent) :
	QRegularExpressionValidator(parent),
	d_ptr(new qREValidatorPrivate(*this))
{
	Q_D(qREValidator);
	d->init();
	d->toolTipDuration = toolTipDuration;
}

qREValidator::qREValidator(
	const QRegularExpression& re, int toolTipDuration, QObject* parent) :
	QRegularExpressionValidator(re, parent),
	d_ptr(new qREValidatorPrivate(*this))
{
	Q_D(qREValidator);
	d->init();
	d->toolTipDuration = toolTipDuration;
}

qREValidator::qREValidator(
	const QRegularExpression& re, QString toolTipText, int toolTipDuration, QObject* parent) :
	QRegularExpressionValidator(re, parent),
	d_ptr(new qREValidatorPrivate(*this))
{
	Q_D(qREValidator);
	d->init();
	d->toolTipDuration = toolTipDuration;
	d->toolTipText = toolTipText;
}

qREValidator::qREValidator(qREValidatorPrivate *pimpl, QObject *parent)
    : QRegularExpressionValidator(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qREValidator::~qREValidator() {

}

QValidator::State qREValidator::validate(
	QString& input, int& pos) const 
{
	Q_D(const qREValidator);

	QValidator::State result = 
		QRegularExpressionValidator::validate(input, pos);

	if (result == QValidator::Invalid && parent() &&
		!d->toolTipText.isEmpty() && d->toolTipDuration > 0) {
		QWidget* widget = static_cast<QWidget*>(parent());
		QToolTip::showText(widget->mapToGlobal(QPoint()),
			d->toolTipText, nullptr, QRect(),
			d->toolTipDuration);
	}

	return result;
}

void qREValidator::setToolTipText(QString str) {
	Q_D(qREValidator);

	d->toolTipText = str;
}

void qREValidator::setToolTipDuration(int val) {
	Q_D(qREValidator);
	d->toolTipDuration = val;
}

QString qREValidator::getToolTipText() {
	Q_D(qREValidator);
	return d->toolTipText;
}

int qREValidator::getToolTipDuration() {
	Q_D(qREValidator);
	return d->toolTipDuration;
}
