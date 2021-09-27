// Colada includes
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

// units include
#include <units/units.hpp>

// PythonQt includes
#include <PythonQt.h>

//using namespace dbcore;

QSqlDatabase dbcore::createDB(QString &fullName) {
  if (QFileInfo(fullName).suffix().compare("db", Qt::CaseInsensitive) != 0) {
    fullName += ".db";
  }
  QFileInfo fi(fullName);
  QString prjName = fi.baseName();
  QString prjPath = fi.absolutePath();
  QDir().mkpath(prjPath);
  return dbcore::createDB(prjPath, prjName);
}

QSqlDatabase dbcore::createDB(const QString &prjPath, QString &prjName) {
  if (QFileInfo(prjName).suffix().compare("db", Qt::CaseInsensitive) != 0) {
    prjName += ".db";
  }
  QDir dir(prjPath + "/" + prjName);
  QString DBFullName = dir.absolutePath();
  QSqlDatabase DB =
      QSqlDatabase::addDatabase("QSQLITE"); // it will be default connection
  DB.setDatabaseName(DBFullName);
  return DB;
}

bool dbcore::createInfoTable() {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return false;

  QString query = "CREATE TABLE IF NOT EXISTS info ("
                  "owner TEXT, "
                  "prj_name TEXT, "
                  "prj_dir TEXT, "
                  "units TEXT, "
                  "crs_auth_name TEXT, "
                  "crs_code TEXT, "
                  "crs_name TEXT)";

  QSqlQuery qry;
  if (!qry.exec(query))
    return false;

  return true;
}

bool dbcore::fillInfoTable(const QString &owner, const QString &prjName,
                           const QString &prjDir, const QString &units,
                           const QString &crsAuthName, const QString &crsCode,
                           const QString &crsName) {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return false;

  QSqlQuery qry;
  qry.prepare("INSERT INTO info ("
              "owner, prj_name, prj_dir, units, crs_auth_name, crs_code, crs_name) "
              "VALUES (?, ?, ?, ?, ?, ?, ?)");

  qry.addBindValue(owner);
  qry.addBindValue(prjName);
  qry.addBindValue(prjDir);
  qry.addBindValue(units);
  qry.addBindValue(crsAuthName);
  qry.addBindValue(crsCode);
  qry.addBindValue(crsName);

  if (!qry.exec())
    return false;

  return true;
}

bool dbcore::createSeisTable() {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return false;

  QString query = "CREATE TABLE seis_containers ("
                  "full_name TEXT)";

  QSqlQuery qry;
  if (!qry.exec(query))
    return false;

  return true;
}

bool dbcore::createWellTable() {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return false;

  QString query = "CREATE TABLE well_containers ("
                  "full_name TEXT)";

  QSqlQuery qry;
  if (!qry.exec(query))
    return false;

  return true;
}

bool dbcore::createHrzTable() {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return false;

  QString query = "CREATE TABLE hrz_containers ("
                  "full_name TEXT)";

  QSqlQuery qry;
  if (!qry.exec(query))
    return false;

  return true;
}

bool dbcore::createProjectFolders(const QString &prjDir) {
  QDir qDir;
  //    QFileInfo fi(QSqlDatabase::database().databaseName());
  //    QString path = fi.path();
  if (!qDir.mkpath(prjDir + "/DATA/seismic"))
    return false;
  if (!qDir.mkpath(prjDir + "/DATA/wells"))
    return false;
  if (!qDir.mkpath(prjDir + "/DATA/horizons"))
    return false;
  return true;
}

bool dbcore::saveSettings() {
  QSettings settings("MyCompany", "MySeismicVision");
  return true;
}

QString dbcore::getCurrentProjectName() {
  // default db is the current project db
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return "";

  return db.databaseName();
}

QString dbcore::getCurrentProjectUnits() {
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return QString();

  QSqlQuery qry(db);
  qry.prepare("SELECT units "
              "FROM info");
  qry.setForwardOnly(true);
  if (!qry.exec())
    return QString();

  QString units;
  while (qry.next()) {
    units = qry.value(0).toString();
  }
  return units;
}

QString dbcore::getCurrentProjectDir() {
  // default db is the current project db
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return "";

  QString dbName = db.databaseName();
  QFileInfo fi(dbName);
  return fi.absolutePath();
}

QString dbcore::getSeisDir() {
  // default db is the current project db
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return "";

  QString dbName = db.databaseName();
  QFileInfo fi(dbName);
  if (!fi.exists())
    return "";

  return fi.absolutePath() + "/DATA/seismic";
}

QString dbcore::getSurfDir() {
  // default db is the current project db
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return "";

  QString dbName = db.databaseName();
  QFileInfo fi(dbName);
  if (!fi.exists())
    return "";

  return fi.absolutePath() + "/DATA/horizons";
}

QString dbcore::getWellDir() {
  // default db is the current project db
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return "";

  QString dbName = db.databaseName();
  QFileInfo fi(dbName);
  if (!fi.exists())
    return "";

  return fi.absolutePath() + "/DATA/wells";
}

QSqlQuery dbcore::getTable(const QString &tableName) {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return QSqlQuery(nullptr);

  QSqlQuery query;
  query.prepare(
      QString(
          "SELECT COUNT(*) FROM sqlite_master WHERE type = table AND name = %1")
          .arg("'" + tableName + "'")); // check if table exists
  query.exec();
  bool tableExist = query.value(0).toBool();
  if (!tableExist) {
    return QSqlQuery(nullptr);
  }

  query.prepare(QString("SELECT * FROM %1").arg("'" + tableName + "'"));
  if (!query.exec()) {
    return QSqlQuery(nullptr);
  }
  return query;
}

QSqlQuery dbcore::getFromTable(const QString &tableName, const QString &colName,
                               const QString &value) {
  if (!QSqlDatabase::database().isOpen() || !QSqlDatabase::database().isValid())
    return QSqlQuery(nullptr);

  QSqlQuery query;
  query.prepare(
      QString(
          "SELECT COUNT(*) FROM sqlite_master WHERE type = table AND name = %1")
          .arg("'" + tableName + "'")); // check if table exists
  query.exec();
  bool tableExist = query.value(0).toBool();
  if (!tableExist) {
    return QSqlQuery(nullptr);
  }

  query.prepare(QString("SELECT * FROM %1 WHERE %2 = %3")
                    .arg("'" + tableName + "'")
                    .arg("'" + colName + "'")
                    .arg("'" + value + "'"));
  if (!query.exec()) {
    return QSqlQuery(nullptr);
  }
  return query;
}

bool dbcore::getAvailableProjections(QStringList &authNameList,
                                     QStringList &codeList,
                                     QStringList &nameList) {
  QSqlDatabase db = QSqlDatabase::database("proj_db");
  if (!db.open())
    return false;

  QSqlQuery qry(db);
  qry.prepare("SELECT auth_name, code, name "
              "FROM projected_crs "
              "WHERE deprecated = 0");
  qry.setForwardOnly(true);
  if (!qry.exec())
    return false;

  while (qry.next()) {
    authNameList.push_back(qry.value(0).toString());
    codeList.push_back(qry.value(1).toString());
    nameList.push_back(qry.value(2).toString());
  }
  return true;
}

bool dbcore::getCurrentProjection(QString &authName, QString &code,
                                  QString &name) {
  QSqlDatabase db = QSqlDatabase::database();
  if (!db.open())
    return false;

  QSqlQuery qry(db);
  qry.prepare("SELECT crs_auth_name, crs_code, crs_name "
              "FROM info");
  qry.setForwardOnly(true);
  if (!qry.exec())
    return false;

  while (qry.next()) {
    authName = qry.value(0).toString();
    code = qry.value(1).toString();
    name = qry.value(2).toString();
  }
  return true;
}

OGRSpatialReference dbcore::getCurrentProjection() {
  QString authName, code, name;
  if (!getCurrentProjection(authName, code, name))
    return OGRSpatialReference();

  /* if project doesn't have specific CRS */
  if (authName.isEmpty() && code.isEmpty())
    return OGRSpatialReference();

  QString units = dbcore::getCurrentProjectUnits();
  double coef = units::convert(units::unit_from_string(units.toStdString()),
      units::unit_from_string("meter"));

  QByteArray authCodeTo = (authName + ":" + code).toUtf8();
  OGRSpatialReference srTo;
  srTo.SetFromUserInput(authCodeTo.data());
  srTo.SetLinearUnitsAndUpdateParameters(units.toUtf8(), coef);

  return srTo;
}

QString dbcore::getCurrentProjectionNameCode() {
  QString authName, code, name;

  if (getCurrentProjection(authName, code, name))
    return authName + ":" + code;
  else
    return QString();
}

bool dbcore::convCoord2CurrentProjection(int nCount, double *x, double *y,
                                         QByteArray authCodeFrom,
                                         const QString &units) {
  if (authCodeFrom.isEmpty())
    return true;

  if (nCount < 1)
    return false;

  double coef = units::convert(units::unit_from_string(units.toStdString()),
                               units::unit_from_string("meter"));

  OGRSpatialReference srFrom;
  srFrom.SetFromUserInput(authCodeFrom.data());
  srFrom.SetLinearUnitsAndUpdateParameters(units.toUtf8(), coef);

  OGRSpatialReference srTo = getCurrentProjection();

  OGRCoordinateTransformation *coordTrans =
      OGRCreateCoordinateTransformation(&srFrom, &srTo);

  if (coordTrans == nullptr)
    return false;

  return coordTrans->Transform(nCount, x, y);
}

bool dbcore::initProjLibDB() {
  std::string env_proj = std::getenv("PROJ_LIB");
  QSqlDatabase DB = QSqlDatabase::addDatabase("QSQLITE", "proj_db");
  DB.setDatabaseName(QString::fromStdString(env_proj) + "/proj.db");
  return true;
}
