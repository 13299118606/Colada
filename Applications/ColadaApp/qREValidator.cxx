// Colada includes
#include "qREValidator.h"
#include "qREValidator_p.h"

// Qt includes
#include <QToolTip>

qREValidatorPrivate::qREValidatorPrivate(qREValidator& q)
	: q_ptr(&q)
{}

qREValidatorPrivate::~qREValidatorPrivate() {}

void qREValidatorPrivate::init(const QString &toolTipText,
                               int toolTipDuration) {
  Q_Q(qREValidator);
  this->toolTipText = toolTipText;
  this->toolTipDuration = toolTipDuration;
}

qREValidator::qREValidator(QObject *parent)
    : QRegularExpressionValidator(parent),
      d_ptr(new qREValidatorPrivate(*this)) {
  Q_D(qREValidator);
  d->init("", 3000);
}

qREValidator::qREValidator(const QString &toolTipText, int toolTipDuration,
                           QObject *parent)
    : QRegularExpressionValidator(parent),
      d_ptr(new qREValidatorPrivate(*this)) {
  Q_D(qREValidator);
  d->init(toolTipText, toolTipDuration);
}

qREValidator::qREValidator(const QRegularExpression &re,
                           const QString &toolTipText,
                           int toolTipDuration, QObject *parent)
    : QRegularExpressionValidator(re, parent),
      d_ptr(new qREValidatorPrivate(*this)) {
  Q_D(qREValidator);
  d->init(toolTipText, toolTipDuration);
}

qREValidator::qREValidator(qREValidatorPrivate *pimpl, QObject *parent)
    : QRegularExpressionValidator(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qREValidator::~qREValidator() {

}

QValidator::State qREValidator::validate(QString &input, int &pos) const {
  Q_D(const qREValidator);

  QValidator::State result = QRegularExpressionValidator::validate(input, pos);

  if (result == QValidator::Invalid && !d->toolTipText.isEmpty() &&
      d->toolTipDuration > 0) {
    QToolTip::showText(QCursor::pos(), d->toolTipText, nullptr, QRect(),
                       d->toolTipDuration);
  }

  return result;
}

void qREValidator::setToolTipText(const QString &str) {
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
