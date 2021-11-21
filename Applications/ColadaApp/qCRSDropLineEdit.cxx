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

  this->NameLineEdit->setPlaceholderText("Name...");
  this->AuthNameLineEdit->setPlaceholderText("Auth name...");
  this->CodeLineEdit->setPlaceholderText("Code...");

  this->NameLineEdit->setReadOnly(true);
  this->AuthNameLineEdit->setReadOnly(true);
  this->CodeLineEdit->setReadOnly(true);

  q->setAcceptDrops(true);
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

  this->setCRSName(name);
  this->setCRSAuthName(authName);
  this->setCRSCode(code.toInt());
}

void qCRSDropLineEdit::setCRSName(QString name){
  Q_D(qCRSDropLineEdit);
  d->NameLineEdit->setText(name);
  emit CRSNameChanged(name);
}

void qCRSDropLineEdit::setCRSAuthName(QString authName){
  Q_D(qCRSDropLineEdit);
  d->AuthNameLineEdit->setText(authName);
  emit CRSAuthNameChanged(authName);
}

void qCRSDropLineEdit::setCRSCode(int code){
  Q_D(qCRSDropLineEdit);
  d->CodeLineEdit->setText(QString::number(code));
  emit CRSCodeChanged(code);
}

QString qCRSDropLineEdit::CRSName(){
  Q_D(qCRSDropLineEdit);
  return d->NameLineEdit->text();
}

QString qCRSDropLineEdit::CRSAuthName(){
  Q_D(qCRSDropLineEdit);
  return d->AuthNameLineEdit->text();
}

int qCRSDropLineEdit::CRSCode(){
  Q_D(qCRSDropLineEdit);
  return d->CodeLineEdit->text().toInt();
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
