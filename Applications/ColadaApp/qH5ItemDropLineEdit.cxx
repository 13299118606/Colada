// Colada includes
#include "qH5ItemDropLineEdit.h"
#include "qH5ItemDropLineEdit_p.h"
#include "qCRSWidget.h"
#include "qColadaH5TreeView.h"
#include "qColadaH5Model.h"

// Qt includes
#include <QLineEdit>
#include <QGridLayout>
#include <QDropEvent>
#include <QMimeData>
#include <QTableView>
#include <QDebug>

qH5ItemDropLineEditPrivate::qH5ItemDropLineEditPrivate(qH5ItemDropLineEdit &q)
    : q_ptr(&q) {}

qH5ItemDropLineEditPrivate::~qH5ItemDropLineEditPrivate() {}

void qH5ItemDropLineEditPrivate::init() {
  Q_Q(qH5ItemDropLineEdit);
  this->gridLayout = new QGridLayout(q);
  this->containerLineEdit = new QLineEdit(q);
  this->objectLineEdit = new QLineEdit(q);

  this->gridLayout->addWidget(this->containerLineEdit, 0, 0);
  this->gridLayout->addWidget(this->objectLineEdit, 0, 1);

  this->containerLineEdit->setPlaceholderText("Container...");
  this->objectLineEdit->setPlaceholderText("Object name...");

  this->containerLineEdit->setReadOnly(true);
  this->objectLineEdit->setReadOnly(true);

  q->setAcceptDrops(true);

  q->connect(containerLineEdit, &QLineEdit::textChanged,
             q, &qH5ItemDropLineEdit::onH5ContainerLineEditChanged);
  q->connect(objectLineEdit, &QLineEdit::textChanged,
             q, &qH5ItemDropLineEdit::onH5ObjectLineEditChanged);
}

qH5ItemDropLineEdit::qH5ItemDropLineEdit(QWidget *parent)
  : QWidget(parent), d_ptr(new qH5ItemDropLineEditPrivate(*this)) {
  Q_D(qH5ItemDropLineEdit);
  d->init();
}

qH5ItemDropLineEdit::qH5ItemDropLineEdit(qH5ItemDropLineEditPrivate *pimpl, QWidget *parent)
    : QWidget(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5ItemDropLineEdit::~qH5ItemDropLineEdit() {

}

void qH5ItemDropLineEdit::setOrientation(Qt::Orientation orientation) {
  Q_D(qH5ItemDropLineEdit);
  if (orientation == Qt::Vertical){
    d->gridLayout->addWidget(d->objectLineEdit, 1, 0);
  } else if (orientation == Qt::Horizontal){
    d->gridLayout->addWidget(d->objectLineEdit, 0, 1);
  }
}

void qH5ItemDropLineEdit::dragEnterEvent(QDragEnterEvent *event) {
  qColadaH5TreeView *sourceTreeView = qobject_cast<qColadaH5TreeView *>(event->source());
  if (sourceTreeView && sourceTreeView->model())
    event->acceptProposedAction();
}

void qH5ItemDropLineEdit::dropEvent(QDropEvent *event) {
  Q_D(qH5ItemDropLineEdit);
  const QMimeData *mime = event->mimeData();

  qColadaH5TreeView *sourceTreeView = qobject_cast<qColadaH5TreeView *>(event->source());
  if (!sourceTreeView ||
      !sourceTreeView->model())
    return;

  QStringList typeList = sourceTreeView->model()->mimeTypes();
  if (typeList.isEmpty())
    return;

  if (!mime->hasFormat(typeList.last())){
    qWarning() << "qH5ItemDropLineEdit::dropEvent: mime doesn have format named: " << typeList.last();
    return;
  }

  QString fileName, objectName;
  QByteArray encodedData = mime->data(typeList.last());
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  stream >> fileName >> objectName;

  this->setH5Data(fileName, objectName);
}

void qH5ItemDropLineEdit::setH5Container(QString name){
  Q_D(qH5ItemDropLineEdit);
  d->containerLineEdit->setText(name);
}

void qH5ItemDropLineEdit::setH5Object(QString name){
  Q_D(qH5ItemDropLineEdit);
  d->objectLineEdit->setText(name);
}

void qH5ItemDropLineEdit::setH5Data(QString container, QString object){
  Q_D(qH5ItemDropLineEdit);
  d->containerLineEdit->blockSignals(true);
  d->objectLineEdit->blockSignals(true);

  d->containerLineEdit->setText(container);
  d->objectLineEdit->setText(object);

  d->containerLineEdit->blockSignals(false);
  d->objectLineEdit->blockSignals(false);

  emit h5ItemChanged(container, object);
}

QString qH5ItemDropLineEdit::h5Container(){
  Q_D(qH5ItemDropLineEdit);
  return d->containerLineEdit->text();
}

QString qH5ItemDropLineEdit::h5Object(){
  Q_D(qH5ItemDropLineEdit);
  return d->objectLineEdit->text();
}

QLineEdit* qH5ItemDropLineEdit::getH5ContainerLineEdit(){
  Q_D(qH5ItemDropLineEdit);
  return d->containerLineEdit;
}

QLineEdit* qH5ItemDropLineEdit::getH5ObjectLineEdit(){
  Q_D(qH5ItemDropLineEdit);
  return d->objectLineEdit;
}

void qH5ItemDropLineEdit::onH5ContainerLineEditChanged(QString name){
  Q_D(qH5ItemDropLineEdit);
  emit h5ItemChanged(name, d->objectLineEdit->text());
}

void qH5ItemDropLineEdit::onH5ObjectLineEditChanged(QString name){
  Q_D(qH5ItemDropLineEdit);
  emit h5ItemChanged(d->containerLineEdit->text(), name);
}
