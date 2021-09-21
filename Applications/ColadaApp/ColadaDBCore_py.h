#ifndef __ColadaDBCore_py_h
#define __ColadaDBCore_py_h

// Colada includes
#include "qColadaAppExport.h"

// Qt includes - need to be included for MOC
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMetaType>

// GDAL includes - need to be included for MOC
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

class QString;
class QStringList;
class QSqlDatabase;
class QSqlQuery;
class OGRSpatialReference;
class QByteArray;


class Q_COLADA_APP_EXPORT ColadaDBCore_py : public QObject {
  Q_OBJECT

public:
  ColadaDBCore_py(QObject *parent = nullptr);
  ~ColadaDBCore_py() = default;

public slots:

  ///
  /// \brief createDB creates new database as a DEFAULT connection
  /// \param fullName
  /// \return NOT opened DB
  ///
  QSqlDatabase createDB(QString &fullName);

  ///
  /// \brief dbcore::createDB creates new database as a DEFAULT connection
  /// \param path path to dest folder
  /// \param name without '.msvprj'
  /// \return NOT opened DB
  ///
  QSqlDatabase createDB(const QString &path, QString &name);

  ///
  /// \brief dbcore::createInfoTable
  /// \return there is only one column 'owner'
  ///
  bool createInfoTable();

  ///
  /// \brief dbcore::fillInfoTable
  /// \param owner owner name
  /// \return
  ///
  bool fillInfoTable(const QString &owner, const QString &prjName,
                     const QString &prjDir, const QString &units,
                     const QString &crsAuthName, const QString &crsCode,
                     const QString &crsName);

  ///
  /// \brief dbcore::createSeisTable
  /// \return there is only one column 'full_name'
  ///
  bool createSeisTable();

  ///
  /// \brief dbcore::createWellTable
  /// \return there is only one column 'full_name'
  ///
  bool createWellTable();

  ///
  /// \brief dbcore::createHrzTable
  /// \return there is only one column 'full_name'
  ///
  bool createHrzTable();
  bool createProjectFolders(const QString &prjDir);

  ///
  /// \brief dbcore::saveSettings
  /// \return IT IS NOT FINISHED
  ///
  bool saveSettings();

  /// @brief return current project fullname
  /// @return
  QString getCurrentProjectName();

  QString getCurrentProjectUnits();

  /// @brief return path where database file (.db) resides
  /// @return
  QString getCurrentProjectDir();
  QString getSeisDir();
  QString getSurfDir();
  QString getWellDir();

  QSqlQuery getTable(const QString &tableName);
  QSqlQuery getFromTable(const QString &tableName, const QString &colName,
                         const QString &value);

  /// @brief return 4 params: `authNameList`, `codeList`, `nameList` and `bool`
  /// @return 
  QVariantList getAvailableProjections();

  /// @brief return 3 params: `authName`, `code` and `name`
  /// Thus QVariantList could be replaced by QStringList
  /// @return
  QVariantList getCurrentProjectionInfo();

  OGRSpatialReference getCurrentProjection();

  QString getCurrentProjectionNameCode();

  QVariantList convCoord2CurrentProjection(double x,
                                           double y,
                                           QByteArray authCodeFrom,
                                           const QString &units);

  /// @brief return 3 params: converted `x_vec`, `y_vec` and `bool` (success or fail)
  /// @param x_vec 
  /// @param y_vec 
  /// @param authCodeFrom 
  /// @param units that corresponds to authCodeFrom
  /// @return 
  QVariantList convCoord2CurrentProjection(std::vector<double>& x_vec,
                                           std::vector<double>& y_vec,
                                           QByteArray authCodeFrom,
                                           const QString &units);

  bool initProjLibDB();
};

Q_DECLARE_METATYPE(std::vector<double>)


#endif