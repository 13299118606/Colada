// Colada includes
#include "qColadaH5WellModel.h"
#include "qColadaH5WellModel_p.h"

// Slicer includes
#include "qSlicerApplication.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLMarkupsNode.h>

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

// h5geo includes
#include <h5geo/h5well.h>
#include <h5geo/h5wellcontainer.h>
#include <h5geo/h5devcurve.h>
#include <h5geo/h5logcurve.h>

qColadaH5WellModelPrivate::qColadaH5WellModelPrivate(qColadaH5WellModel &q)
    : Superclass(q) {}

//-----------------------------------------------------------------------------
qColadaH5WellModelPrivate::~qColadaH5WellModelPrivate() {}

void qColadaH5WellModelPrivate::init(const QString &title) {
  Q_Q(qColadaH5WellModel);
  this->Superclass::init(title);
}

qColadaH5WellModel::qColadaH5WellModel(QObject *parent)
    : Superclass(new qColadaH5WellModelPrivate(*this), parent) {
  Q_D(qColadaH5WellModel);
  d->init("");
}

qColadaH5WellModel::qColadaH5WellModel(const QString &title, QObject *parent)
    : Superclass(new qColadaH5WellModelPrivate(*this), parent) {
  Q_D(qColadaH5WellModel);
  d->init(title);
}

qColadaH5WellModel::qColadaH5WellModel(qColadaH5WellModelPrivate *pimpl,
                                       QObject *parent)
    : Superclass(pimpl, parent) {
  // init() is called by derived class.
}

qColadaH5WellModel::~qColadaH5WellModel() {}

QVariant qColadaH5WellModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::CheckStateRole)
    if (wellFromItem(itemFromIndex(index)) == nullptr &&
        devCurveFromItem(itemFromIndex(index)) == nullptr &&
        logCurveFromItem(itemFromIndex(index)) == nullptr)
      return QVariant();

  return qColadaH5Model::data(index, role);
}

bool qColadaH5WellModel::hasChildren(const QModelIndex &parent) const {
  qColadaH5Item *parentItem = itemFromIndex(parent);
  H5DevCurve *devCurve = devCurveFromItem(parentItem);
  if (devCurve)
    return false;

  H5LogCurve *logCurve = logCurveFromItem(parentItem);
  if (logCurve)
    return false;

  return qColadaH5Model::hasChildren(parent);
}

void qColadaH5WellModel::fetchMore(const QModelIndex &parent) {
  qColadaH5Item *parentItem = itemFromIndex(parent);

  QVector<qColadaH5Item *> childItems;
  if (parentItem->isGeoContainer()) {
    H5BaseContainer *obj = parentItem->getGeoContainer();
    if (obj == nullptr)
      return;

    h5gt::File file = obj->getH5File();
    std::vector<std::string> childrenNameList = file.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (file.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = file.getGroup(childrenNameList[i]);     
      if (h5geo::isWell(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openWell(group), parentItem));
      } else {
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openBaseObject(group), parentItem));
      }
    }
  } else if (parentItem->isGeoObject()) {
    H5BaseObject *obj = parentItem->getGeoObject();
    if (obj == nullptr ||
        dynamic_cast<H5DevCurve *>(obj) ||
        dynamic_cast<H5LogCurve *>(obj))
      return;

    h5gt::Group objG = obj->getObjG();
    std::vector<std::string> childrenNameList = objG.listObjectNames();

    childItems.reserve(childrenNameList.size());
    for (int i = 0; i < childrenNameList.size(); i++) {
      if (objG.getObjectType(childrenNameList[i]) != h5gt::ObjectType::Group)
        continue;

      h5gt::Group group = objG.getGroup(childrenNameList[i]);

      if (h5geo::isWell(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openWell(group), parentItem));
      } else if (h5geo::isDevCurve(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openDevCurve(group), parentItem));
      } else if (h5geo::isLogCurve(group)){
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openLogCurve(group), parentItem));
      } else {
        childItems.push_back(
              new qColadaH5Item(
                h5geo::openBaseObject(group), parentItem));
      }
    }
  }
  childItems.shrink_to_fit();

  if (childItems.isEmpty())
    return;

  int insrtCnt = childItems.count() - 1;
  beginInsertRows(parent, 0, insrtCnt);
  parentItem->setChildren(childItems);
  endInsertRows();
  parentItem->setMapped(true);
}

Qt::ItemFlags qColadaH5WellModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flags =
      Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;

  qColadaH5Item *item = itemFromIndex(index);
  if (wellFromItem(item) || devCurveFromItem(item) || logCurveFromItem(item))
    flags |= Qt::ItemIsUserCheckable;

  return flags;
}

bool qColadaH5WellModel::canAddH5File(h5gt::File file) const {
  H5WellCnt_ptr wellCnt(h5geo::openWellContainer(file));
  if (wellCnt == nullptr)
    return false;
  return true;
}

H5Well *qColadaH5WellModel::wellFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5Well *>(item->getGeoObject());
}

H5WellContainer *
qColadaH5WellModel::wellCntFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5WellContainer *>(item->getGeoContainer());
}

H5DevCurve *qColadaH5WellModel::devCurveFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5DevCurve *>(item->getGeoObject());
}

H5LogCurve *qColadaH5WellModel::logCurveFromItem(qColadaH5Item *item) const {
  return dynamic_cast<H5LogCurve *>(item->getGeoObject());
}

void qColadaH5WellModel::onMRMLSceneNodeAdded(vtkObject*, vtkObject* node)
{
  Q_D(qColadaH5WellModel);
  this->qColadaH5Model::onMRMLSceneNodeAdded(nullptr, node);

  vtkMRMLMarkupsNode* markupNode = vtkMRMLMarkupsNode::SafeDownCast(node);
  if (markupNode && std::string(markupNode->GetName()) == "well_tops"){
    qvtkConnect(markupNode, vtkMRMLMarkupsNode::PointAddedEvent,
                   this, SLOT(onMarkupPointAdded(vtkObject*, vtkObject*)));
    qvtkConnect(markupNode, vtkMRMLMarkupsNode::PointRemovedEvent,
                   this, SLOT(onMarkupPointRemoved(vtkObject*, vtkObject*)));
  }
}

void qColadaH5WellModel::onMarkupPointAdded(vtkObject* caller, int pid)
{
  Q_D(qColadaH5WellModel);
  vtkMRMLMarkupsNode* markupNode =
      vtkMRMLMarkupsNode::SafeDownCast(caller);
  if (!markupNode)
    return;

  if (pid < 0 ||
      pid >= markupNode->GetNumberOfControlPoints())
    return;

  auto controlPoint = markupNode->GetNthControlPoint(pid);
  QString description = QString::fromStdString(controlPoint->Description);

  QString fileName, objName;
  getContainerAndObjNamesFromControlPointDesription(
        description, fileName, objName);

  if (fileName.isEmpty() ||
      objName.isEmpty())
    return;

  qColadaH5Item* item = findItemByContainerAndObjectNames(fileName, objName);
  if (!item)
    return;

  item->setCheckState(Qt::Checked);
  QModelIndex index = getIndex(item);
  emit dataChanged(index, index, {Qt::CheckStateRole});
}

void qColadaH5WellModel::onMarkupPointRemoved(vtkObject* caller, int pid)
{
  Q_D(qColadaH5WellModel);
  vtkMRMLMarkupsNode* markupNode =
      vtkMRMLMarkupsNode::SafeDownCast(caller);
  if (!markupNode)
    return;

  if (pid < 0 ||
      pid >= markupNode->GetNumberOfControlPoints())
    return;

  auto controlPoint = markupNode->GetNthControlPoint(pid);
  QString description = QString::fromStdString(controlPoint->Description);

  QString fileName, objName;
  getContainerAndObjNamesFromControlPointDesription(
        description, fileName, objName);

  if (fileName.isEmpty() ||
      objName.isEmpty())
    return;

  qColadaH5Item* item = findItemByContainerAndObjectNames(fileName, objName);
  if (!item)
    return;

  item->setCheckState(Qt::Unchecked);
  QModelIndex index = getIndex(item);
  emit dataChanged(index, index, {Qt::CheckStateRole});
}

void qColadaH5WellModel::getContainerAndObjNamesFromControlPointDesription(
    const QString &description,
    QString& fileName, QString& objName)
{
  QStringList lines = description.split("\n");

  for (const auto& line : lines){
    QStringList strings = line.split(" :");

    if (strings.count() != 2)
      continue;

    if (strings[0].contains("GeoContainer")){
      fileName = strings[1];
    } else if (strings[0].contains("GeoObject")){
      objName = strings[1];
    }

    if (!fileName.isEmpty() &&
        !objName.isEmpty())
      return;
  }
}
