// Colada includes
#include "qCRSDropLineEdit.h"
#include "qCRSDropLineEdit_p.h"
#include "qCRSWidget.h"

// Qt includes
#include <QLineEdit>
#include <QHBoxLayout>
#include <QDropEvent>
#include <QMimeData>
#include <QTableView>
#include <QDebug>

qCRSDropLineEditPrivate::qCRSDropLineEditPrivate(qCRSDropLineEdit &q)
    : q_ptr(&q) {}

qCRSDropLineEditPrivate::~qCRSDropLineEditPrivate() {}

void qCRSDropLineEditPrivate::init() {
  Q_Q(qCRSDropLineEdit);
  this->HLayout = new QHBoxLayout(q);
  this->NameLineEdit = new QLineEdit(q);
  this->AuthNameLineEdit = new QLineEdit(q);
  this->CodeLineEdit = new QLineEdit(q);

  this->HLayout->addWidget(this->NameLineEdit);
  this->HLayout->addWidget(this->AuthNameLineEdit);
  this->HLayout->addWidget(this->CodeLineEdit);

  this->HLayout->setMargin(0);

  this->NameLineEdit->setPlaceholderText("Name...");
  this->AuthNameLineEdit->setPlaceholderText("Auth name...");
  this->CodeLineEdit->setPlaceholderText("Code...");

  this->NameLineEdit->setReadOnly(true);
  this->AuthNameLineEdit->setReadOnly(true);
  this->CodeLineEdit->setReadOnly(true);

  q->setAcceptDrops(true);

  q->connect(NameLineEdit, &QLineEdit::textChanged,
             q, &qCRSDropLineEdit::onNameLineEditChanged);
  q->connect(AuthNameLineEdit, &QLineEdit::textChanged,
             q, &qCRSDropLineEdit::onAuthNameLineEditChanged);
  q->connect(CodeLineEdit, &QLineEdit::textChanged,
             q, &qCRSDropLineEdit::onCodeLineEditChanged);
}

qCRSDropLineEdit::qCRSDropLineEdit(QWidget *parent)
  : QWidget(parent), d_ptr(new qCRSDropLineEditPrivate(*this)) {
  Q_D(qCRSDropLineEdit);
  d->init();
}

qCRSDropLineEdit::qCRSDropLineEdit(qCRSDropLineEditPrivate *pimpl, QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qCRSDropLineEdit::~qCRSDropLineEdit() {

}

void qCRSDropLineEdit::dragEnterEvent(QDragEnterEvent *event) {
  QTableView *sourceTableView = qobject_cast<QTableView *>(event->source());
  if (sourceTableView && sourceTableView->model())
    event->acceptProposedAction();
}

void qCRSDropLineEdit::dropEvent(QDropEvent *event) {
  const QMimeData *mime = event->mimeData();

  QTableView *sourceTableView = qobject_cast<QTableView *>(event->source());
  if (!sourceTableView ||
      !sourceTableView->model())
    return;

  QStringList typeList = sourceTableView->model()->mimeTypes();
  if (typeList.isEmpty())
    return;

  if (!mime->hasFormat(typeList[0])){
    qWarning() << "qCRSDropLineEdit::dropEvent: mime doesn have format named: " << typeList.last();
    return;
  }

  int sourceRow, sourceCol;
  QByteArray encodedData = mime->data(typeList[0]);
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  stream >> sourceRow >> sourceCol;

  if (sourceTableView->model()->rowCount() <= sourceRow ||
      sourceTableView->model()->columnCount() < 3)
    return;

  QString name = sourceTableView->model()->
      data(sourceTableView->model()->index(sourceRow, 0)).toString();
  QString authName = sourceTableView->model()->
      data(sourceTableView->model()->index(sourceRow, 1)).toString();
  QString code = sourceTableView->model()->
      data(sourceTableView->model()->index(sourceRow, 2)).toString();

  this->setCRS(name, authName, code);
}

void qCRSDropLineEdit::setCRS(
    const QString& name,
    const QString& authName,
    const QString& code)
{
  Q_D(qCRSDropLineEdit);
  d->NameLineEdit->blockSignals(true);
  d->AuthNameLineEdit->blockSignals(true);
  d->CodeLineEdit->blockSignals(true);

  d->NameLineEdit->setText(name);
  d->AuthNameLineEdit->setText(authName);
  d->CodeLineEdit->setText(code);

  d->NameLineEdit->blockSignals(false);
  d->AuthNameLineEdit->blockSignals(false);
  d->CodeLineEdit->blockSignals(false);

  // All the four signals must be emitted saving in settings
  // claims CRSNameChanged, CRSAuthNameChanged, CRSCodeChanged to be emitted
  // and CRSChanged is more convenient signal for other widgets
  emit CRSNameChanged(name);
  emit CRSAuthNameChanged(authName);
  emit CRSCodeChanged(code);
  emit CRSChanged(name, authName, code);
}

void qCRSDropLineEdit::setCRSName(const QString& name){
  Q_D(qCRSDropLineEdit);
  d->NameLineEdit->setText(name);
}

void qCRSDropLineEdit::setCRSAuthName(const QString& authName){
  Q_D(qCRSDropLineEdit);
  d->AuthNameLineEdit->setText(authName);
}

void qCRSDropLineEdit::setCRSCode(const QString& code){
  Q_D(qCRSDropLineEdit);
  d->CodeLineEdit->setText(code);
}

QString qCRSDropLineEdit::CRSName(){
  Q_D(qCRSDropLineEdit);
  return d->NameLineEdit->text();
}

QString qCRSDropLineEdit::CRSAuthName(){
  Q_D(qCRSDropLineEdit);
  return d->AuthNameLineEdit->text();
}

QString qCRSDropLineEdit::CRSCode(){
  Q_D(qCRSDropLineEdit);
  return d->CodeLineEdit->text();
}

QLineEdit* qCRSDropLineEdit::getCRSNameLineEdit(){
  Q_D(qCRSDropLineEdit);
  return d->NameLineEdit;
}

QLineEdit* qCRSDropLineEdit::getCRSAuthNameLineEdit(){
  Q_D(qCRSDropLineEdit);
  return d->AuthNameLineEdit;
}

QLineEdit* qCRSDropLineEdit::getCRSCodeLineEdit(){
  Q_D(qCRSDropLineEdit);
  return d->CodeLineEdit;
}

void qCRSDropLineEdit::onNameLineEditChanged(
    const QString& name)
{
  Q_D(qCRSDropLineEdit);
  emit CRSChanged(
        name,
        d->AuthNameLineEdit->text(),
        d->CodeLineEdit->text());
  emit CRSNameChanged(name);
}

void qCRSDropLineEdit::onAuthNameLineEditChanged(
    const QString& authName)
{
  Q_D(qCRSDropLineEdit);
  emit CRSChanged(
        d->NameLineEdit->text(),
        authName,
        d->CodeLineEdit->text());
  emit CRSAuthNameChanged(authName);
}

void qCRSDropLineEdit::onCodeLineEditChanged(
    const QString& code)
{
  Q_D(qCRSDropLineEdit);
  emit CRSChanged(
        d->NameLineEdit->text(),
        d->AuthNameLineEdit->text(),
        code);
  emit CRSCodeChanged(code);
}
