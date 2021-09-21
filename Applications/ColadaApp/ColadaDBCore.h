#ifndef __qColadaDBCore_h
#define __qColadaDBCore_h

// Colada includes
#include "qColadaAppExport.h"

class QString;
class QStringList;
class QSqlDatabase;
class QSqlQuery;
class OGRSpatialReference;
class QByteArray;


namespace dbcore {
///
/// \brief createDB creates new database as a DEFAULT connection
/// \param fullName
/// \return NOT opened DB
///
QSqlDatabase Q_COLADA_APP_EXPORT createDB(QString &fullName);

///
/// \brief dbcore::createDB creates new database as a DEFAULT connection
/// \param path path to dest folder
/// \param name without '.msvprj'
/// \return NOT opened DB
///
QSqlDatabase Q_COLADA_APP_EXPORT createDB(const QString &path, QString &name);

///
/// \brief dbcore::createInfoTable
/// \return there is only one column 'owner'
///
bool Q_COLADA_APP_EXPORT createInfoTable();

///
/// \brief dbcore::fillInfoTable
/// \param owner owner name
/// \return
///
bool Q_COLADA_APP_EXPORT fillInfoTable(
    const QString &owner, const QString &prjName, const QString &prjDir,
    const QString &units, const QString &crsAuthName, const QString &crsCode,
    const QString &crsName);

///
/// \brief dbcore::createSeisTable
/// \return there is only one column 'full_name'
///
bool Q_COLADA_APP_EXPORT createSeisTable();

///
/// \brief dbcore::createWellTable
/// \return there is only one column 'full_name'
///
bool Q_COLADA_APP_EXPORT createWellTable();

///
/// \brief dbcore::createHrzTable
/// \return there is only one column 'full_name'
///
bool Q_COLADA_APP_EXPORT createHrzTable();
bool Q_COLADA_APP_EXPORT createProjectFolders(const QString &prjDir);

///
/// \brief dbcore::saveSettings
/// \return IT IS NOT FINISHED
///
bool Q_COLADA_APP_EXPORT saveSettings();

/// @brief return current project fullname
/// @return 
QString Q_COLADA_APP_EXPORT getCurrentProjectName();

QString Q_COLADA_APP_EXPORT getCurrentProjectUnits();

/// @brief return path where database file (.db) resides
/// @return 
QString Q_COLADA_APP_EXPORT getCurrentProjectDir();
QString Q_COLADA_APP_EXPORT getSeisDir();
QString Q_COLADA_APP_EXPORT getSurfDir();
QString Q_COLADA_APP_EXPORT getWellDir();

QSqlQuery Q_COLADA_APP_EXPORT getTable(const QString &tableName);
QSqlQuery Q_COLADA_APP_EXPORT getFromTable(const QString &tableName,
                                           const QString &colName,
                                           const QString &value);

bool Q_COLADA_APP_EXPORT getAvailableProjections(QStringList &authNameList,
                                                 QStringList &codeList,
                                                 QStringList &nameList);

bool Q_COLADA_APP_EXPORT getCurrentProjection(QString &authName, QString &code,
                                              QString &name);

OGRSpatialReference Q_COLADA_APP_EXPORT getCurrentProjection();

QString Q_COLADA_APP_EXPORT getCurrentProjectionNameCode();

bool Q_COLADA_APP_EXPORT convCoord2CurrentProjection(int nCount, double *x,
                                                     double *y,
                                                     QByteArray authCodeFrom,
                                                     const QString &units);

// I NEED TO PROPERLY SET PATH TO PROJ_DB
bool Q_COLADA_APP_EXPORT initProjLibDB();


} // namespace dbcore


#endif