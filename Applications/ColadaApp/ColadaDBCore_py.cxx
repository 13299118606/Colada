// Colada includes
#include "ColadaDBCore_py.h"
#include "ColadaDBCore.h"

// Qt includes
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>

// GDAL includes
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

// PythonQt includes
#include <PythonQt.h>

ColadaDBCore_py::ColadaDBCore_py(QObject *parent) : QObject(parent) {}

QSqlDatabase ColadaDBCore_py::createDB(QString &fullName) {
  return dbcore::createDB(fullName);
}

QSqlDatabase ColadaDBCore_py::createDB(const QString &path, QString &name) {
  return dbcore::createDB(path, name);
}

bool ColadaDBCore_py::createInfoTable() { return dbcore::createInfoTable(); }

bool ColadaDBCore_py::fillInfoTable(const QString &owner,
                                    const QString &prjName,
                                    const QString &prjDir, const QString &units,
                                    const QString &crsAuthName,
                                    const QString &crsCode,
                                    const QString &crsName) {
  return dbcore::fillInfoTable(owner, prjName, prjDir, units, crsAuthName,
                               crsCode, crsName);
}

bool ColadaDBCore_py::createSeisTable() { return dbcore::createSeisTable(); }

bool ColadaDBCore_py::createWellTable() { return dbcore::createWellTable(); }

bool ColadaDBCore_py::createHrzTable() { return dbcore::createHrzTable(); }

bool ColadaDBCore_py::createProjectFolders(const QString &prjDir) {
  return dbcore::createProjectFolders(prjDir);
}

bool ColadaDBCore_py::saveSettings() {
  QSettings settings("MyCompany", "MySeismicVision");
  return true;
}

QString ColadaDBCore_py::getCurrentProjectName() {
  return dbcore::getCurrentProjectName();
}

QString ColadaDBCore_py::getCurrentProjectUnits() {
  return dbcore::getCurrentProjectUnits();
}

QString ColadaDBCore_py::getCurrentProjectDir() {
  return dbcore::getCurrentProjectDir();
}

QString ColadaDBCore_py::getSeisDir() { return dbcore::getSeisDir(); }

QString ColadaDBCore_py::getSurfDir() { return dbcore::getSurfDir(); }

QString ColadaDBCore_py::getWellDir() { return dbcore::getWellDir(); }

QSqlQuery ColadaDBCore_py::getTable(const QString &tableName) {
  return dbcore::getTable(tableName);
}

QSqlQuery ColadaDBCore_py::getFromTable(const QString &tableName,
                                        const QString &colName,
                                        const QString &value) {
  return dbcore::getFromTable(tableName, colName, value);
}

QVariantList ColadaDBCore_py::getAvailableProjections() {
  QStringList authNameList, codeList, nameList;
  bool val = dbcore::getAvailableProjections(authNameList, codeList, nameList);
  return QVariantList({authNameList, codeList, nameList, val});
}

QVariantList ColadaDBCore_py::getCurrentProjectionInfo() {
  QString authName, code, name;
  dbcore::getCurrentProjection(authName, code, name);
  return QVariantList({authName, code, name});
}

OGRSpatialReference ColadaDBCore_py::getCurrentProjection() {
  return dbcore::getCurrentProjection();
}

QString ColadaDBCore_py::getCurrentProjectionNameCode() {
  return dbcore::getCurrentProjectionNameCode();
}

QVariantList ColadaDBCore_py::convCoord2CurrentProjection(
    double x, double y, QByteArray authCodeFrom, const QString &units) {
  bool val =
      dbcore::convCoord2CurrentProjection(1, &x, &y, authCodeFrom, units);
  return QVariantList({x, y, val});
}

QVariantList ColadaDBCore_py::convCoord2CurrentProjection(
    std::vector<double> &x_vec, std::vector<double> &y_vec,
    QByteArray authCodeFrom, const QString &units) {
  if (x_vec.size() != y_vec.size())
    return QVariantList({0, 0, false});

  bool val = dbcore::convCoord2CurrentProjection(
      x_vec.size(), x_vec.data(), y_vec.data(), authCodeFrom, units);
  QVariant var1, var2, var3;
  var1.setValue(x_vec);
  var2.setValue(y_vec);
  var3.setValue(val);
  return QVariantList({var1, var2, var3});
}

bool ColadaDBCore_py::initProjLibDB() { return dbcore::initProjLibDB(); }