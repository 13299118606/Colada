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

// CTK includes
#include <ctkPathLineEdit.h>

// h5geo includes
#include<h5geo/h5seiscontainer.h>
#include <h5geo/h5seis.h>
#include <h5geo/h5mapcontainer.h>
#include <h5geo/h5map.h>
#include <h5geo/h5wellcontainer.h>
#include <h5geo/h5well.h>
#include <h5geo/h5devcurve.h>
#include <h5geo/h5logcurve.h>
#include <h5geo/h5points.h>

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

qH5ItemDropLineEditPrivate::qH5ItemDropLineEditPrivate(qH5ItemDropLineEdit &q)
    : q_ptr(&q) {}

qH5ItemDropLineEditPrivate::~qH5ItemDropLineEditPrivate() {}

void qH5ItemDropLineEditPrivate::init() {
  Q_Q(qH5ItemDropLineEdit);
  this->gridLayout = new QGridLayout(q);
  this->gridLayout->setMargin(0);

  this->containerPathLineEdit = new ctkPathLineEdit(q);
  this->containerPathLineEdit->setNameFilters(
        QStringList() << "hdf5 (*.h5 *.hdf5)" <<  "all (*.*)");
  this->objectLineEdit = new QLineEdit(q);

  // adds widget to grid layout
  q->setOrientation(Qt::Vertical);

  QLineEdit* containerLineEdit = q->getH5ContainerLineEdit();
  if (!containerLineEdit){
    qCritical() << Q_FUNC_INFO << "Unable to find container line edit";
    return;
  }

  containerLineEdit->setPlaceholderText("Container...");
  this->objectLineEdit->setPlaceholderText("Object name...");

  containerLineEdit->setReadOnly(false);
  this->objectLineEdit->setReadOnly(false);

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

H5BaseContainer* qH5ItemDropLineEdit::openGeoContainer(){
  Q_D(qH5ItemDropLineEdit);
  return h5geo::openContainerByName(h5Container().toStdString());
}

H5BaseObject* qH5ItemDropLineEdit::openGeoObject(){
  Q_D(qH5ItemDropLineEdit);
  H5BaseCnt_ptr cnt(openGeoContainer());
  if (!cnt)
    return nullptr;

  auto file = cnt->getH5File();
  std::string objName = h5Object().toStdString();
  if (!file.hasObject(objName, h5gt::ObjectType::Group))
    return nullptr;

  auto group = file.getGroup(objName);
  return h5geo::openObject(group);
}

void qH5ItemDropLineEdit::setOrientation(Qt::Orientation orientation) {
  Q_D(qH5ItemDropLineEdit);
  if (orientation == Qt::Vertical){
    d->gridLayout->addWidget(d->containerPathLineEdit, 0, 0);
    d->gridLayout->addWidget(d->objectLineEdit, 1, 0);
  } else if (orientation == Qt::Horizontal){
    d->gridLayout->addWidget(d->containerPathLineEdit, 0, 0);
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
  if (!event)
    return;

  qColadaH5TreeView *sourceTreeView = qobject_cast<qColadaH5TreeView *>(event->source());
  if (!sourceTreeView ||
      !sourceTreeView->model())
    return;

  QStringList typeList = sourceTreeView->model()->mimeTypes();
  if (typeList.isEmpty())
    return;

  const QMimeData *mime = event->mimeData();
  if (!mime || !mime->hasFormat(typeList.last()))
    return;

  QString fileName, objectName;
  QByteArray encodedData = mime->data(typeList.last());
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  stream >> fileName >> objectName;

  this->setH5Data(fileName, objectName);
}

void qH5ItemDropLineEdit::setH5Container(const QString &name){
  Q_D(qH5ItemDropLineEdit);
  d->containerPathLineEdit->setCurrentPath(name);
}

void qH5ItemDropLineEdit::setH5Object(const QString& name){
  Q_D(qH5ItemDropLineEdit);
  d->objectLineEdit->setText(name);
}

void qH5ItemDropLineEdit::setH5Data(const QString& container, const QString& object){
  Q_D(qH5ItemDropLineEdit);
  QLineEdit* containerLineEdit = this->getH5ContainerLineEdit();
  if (!containerLineEdit){
    qCritical() << Q_FUNC_INFO << "Unable to find container line edit";
    return;
  }

  d->containerPathLineEdit->blockSignals(true);
  containerLineEdit->blockSignals(true);
  d->objectLineEdit->blockSignals(true);

  d->containerPathLineEdit->setCurrentPath(container);
  d->objectLineEdit->setText(object);

  d->containerPathLineEdit->blockSignals(false);
  containerLineEdit->blockSignals(false);
  d->objectLineEdit->blockSignals(false);

  emit h5ItemChanged(container, object);
}

QString qH5ItemDropLineEdit::h5Container(){
  Q_D(qH5ItemDropLineEdit);
  return d->containerPathLineEdit->currentPath();
}

QString qH5ItemDropLineEdit::h5Object(){
  Q_D(qH5ItemDropLineEdit);
  return d->objectLineEdit->text();
}

ctkPathLineEdit* qH5ItemDropLineEdit::getH5ContainerPathLineEdit(){
  Q_D(qH5ItemDropLineEdit);
  return d->containerPathLineEdit;
}

QLineEdit* qH5ItemDropLineEdit::getH5ContainerLineEdit(){
  Q_D(qH5ItemDropLineEdit);
  return d->containerPathLineEdit->findChild<QLineEdit*>();
}

QLineEdit* qH5ItemDropLineEdit::getH5ObjectLineEdit(){
  Q_D(qH5ItemDropLineEdit);
  return d->objectLineEdit;
}

void qH5ItemDropLineEdit::onH5ContainerLineEditChanged(
    const QString& name)
{
  Q_D(qH5ItemDropLineEdit);
  emit h5ItemChanged(name, d->objectLineEdit->text());
}

void qH5ItemDropLineEdit::onH5ObjectLineEditChanged(
    const QString& name)
{
  Q_D(qH5ItemDropLineEdit);
  emit h5ItemChanged(d->containerPathLineEdit->currentPath(), name);
}
