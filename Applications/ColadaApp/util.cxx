#include "util.h"

// Qt includes
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>

// Slicer includes
#include "qSlicerApplication.h"

// h5geo includes
#include <h5geo/h5core.h>

// GDAL includes
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

using namespace util;

QRegularExpression util::fileNameRegExp() {
  const QRegularExpression re(R"RX(\A(?!(?:COM[0-9]|CON|LPT[0-9]|NUL|PRN|AUX|com[0-9]|con|lpt[0-9]|nul|prn|aux)|\s|[\.]{2,})[^\\\/:*"?<>|]{1,254}(?<![\s\.])\z)RX");
  return re;
}

QRegularExpression util::fileNameNoSpaceRegExp() {
  const QRegularExpression re(R"RX(\A(?!(?:COM[0-9]|CON|LPT[0-9]|NUL|PRN|AUX|com[0-9]|con|lpt[0-9]|nul|prn|aux)|[\.]{2,})[^\\\/:*"?<>|\s]{1,254}(?<![\.])\z)RX");
  return re;
}

QRegExp util::floatRegExp() {
  const QRegExp re("[+-]?([0-9]*[.])?[0-9]+");
  return re;
}

QString util::fileNameToolTipText() {
  QString str(R"(File name must not contain the following characters: \ / : * ? " < > |)");
  return str;
}

QString util::fileNameNoSpaceToolTipText() {
  QString str(R"(File name must not contain the following characters: \ / : * ? " < > | or white-space)");
  return str;
}

QStringList util::readTxtFileByLines(const QString &filePath) {
  QFile qFile(filePath);
  if (!qFile.open(QFile::ReadOnly | QFile::Text))
    return QStringList();

  QStringList traceHeaderNames;
  QTextStream in(&qFile);
  while (!in.atEnd()) {
    QString line = in.readLine();
    traceHeaderNames.push_back(line); //specifying number = # of characters
  }
  qFile.close();

  return traceHeaderNames;
}

bool util::removeDir(const QString& dirName)
{
  bool result = true;
  QDir dir(dirName);

  if (dir.exists(dirName)) {
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                                QDir::Hidden | QDir::AllDirs |
                                                QDir::Files, QDir::DirsFirst)) {
      if (info.isDir()) {
        result = removeDir(info.absoluteFilePath());
      }
      else {
        result = QFile::remove(info.absoluteFilePath());
      }

      if (!result) {
        return result;
      }
    }
    result = dir.rmdir(dirName);
  }

  return result;
}

QString util::CRSName()
{
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString crsName = appSettings->value("Projection/CRSName", "").toString();
  return crsName;
}

QString util::CRSAuthName()
{
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString crsAuthName = appSettings->value("Projection/CRSAuthName", "").toString();
  return crsAuthName;
}

int util::CRSCode()
{
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString crsCode = appSettings->value("Projection/CRSCode", "").toString();
  return crsCode.toInt();
}

QString util::LengthUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Length/suffix", "").toString();
  return units;
}

QString util::TimeUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Time/suffix", "").toString();
  return units;
}

QString util::FrequencyUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Frequency/suffix", "").toString();
  return units;
}

QString util::VelocityUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Velocity/suffix", "").toString();
  return units;
}

QString util::IntensityUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Intensity/suffix", "").toString();
  return units;
}

QString util::AreaUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Area/suffix", "").toString();
  return units;
}

QString util::VolumeUnits(){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  QString units = appSettings->value("Volume/suffix", "").toString();
  return units;
}

bool util::getAvailableProjections(
    QStringList &authNameList,
    QStringList &codeList,
    QStringList &nameList){
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

void util::getCurrentProjection(
    QString &authName,
    QString &code,
    QString &name){
  qSlicerApplication* app = qSlicerApplication::application();
  QSettings* appSettings = app->userSettings();
  Q_ASSERT(appSettings);
  authName = appSettings->value("Projection/CRSAuthName", "").toString();
  code = appSettings->value("Projection/CRSCode", "").toString();
  name = appSettings->value("Projection/CRSName", "").toString();
}

OGRSpatialReference util::getCurrentProjection(){
  QString authName, code, name;
  getCurrentProjection(authName, code, name);

  /* if project doesn't have specific CRS */
  if (authName.isEmpty() && code.isEmpty())
    return OGRSpatialReference();

  // OGR assumes that we set coef related to the meter
  QString units = util::LengthUnits();
  double coef = units::convert(units::unit_from_string(units.toStdString()),
      units::unit_from_string("meter"));

  QByteArray authCodeTo = (authName + ":" + code).toUtf8();
  OGRSpatialReference srTo;
  srTo.SetFromUserInput(authCodeTo.data());
  srTo.SetLinearUnitsAndUpdateParameters(units.toUtf8(), coef);

  return srTo;
}

bool util::convCoord2CurrentProjection(
    int nCount,
    double *x,
    double *y,
    QByteArray authCodeFrom,
    const QString &units){
  if (authCodeFrom.isEmpty())
    return true;

  if (nCount < 1)
    return false;

  double coef = units::convert(
        units::unit_from_string(units.toStdString()),
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

bool util::initProjLibDB() {
  std::string env_proj = std::getenv("PROJ_LIB");
  QSqlDatabase DB = QSqlDatabase::addDatabase("QSQLITE", "proj_db");
  DB.setDatabaseName(QString::fromStdString(env_proj) + "/proj.db");
  return true;
}
