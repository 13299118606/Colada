#ifndef __qColadaAppPythonQtDecorators_h
#define __qColadaAppPythonQtDecorators_h

#include "PythonQt.h"
#include "PythonQtCppWrapperFactory.h"

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkCheckableHeaderView.h>

// Colada includes
#include "dbcore.h"
#include "util.h"

// Colada includes
#include "qColadaAppExport.h"

// Qt includes - need to be included for MOC
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QMetaType>
#include <QRegularExpression>

// GDAL includes - need to be included for MOC
#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>

// units includes
#include <units/units.hpp>



#include <h5gt/H5File.hpp>
#include <h5geo/h5seiscontainer.h>


/* Rules to create bindings via PythonQt:
   - don`t write `~MyClass = default`;
   - don`t use EXPORT directive for a decorator class;
   - use name conventions when declaring methods of decorator`s class
   to overload constructor or add static methods (new_MyClass or static_MyMethod)
   - `PythonQt::self()->addDecorators(...)` automatically adds `registerClass` but
   to set your own module name you should better manually write `PythonQt::self()->registerClass(...)`
   - all decorator staff and its initialization implement in HEADER file or you will get multiple function definition error
   - see more info at: https://doc.qt.io/archives/qq/qq23-pythonqt.html#:~:text=a%20%3D%20QSize(1%2C2)-,Decorators%20and%20C%2B%2B%20Wrappers,-A%20major%20problem
   - Slicer automatically passes classes to PythonQt that are named the exactly the same as the filename
*/

class DBCore {
//  Q_OBJECT
};

class Util {
//  Q_OBJECT
};

class qColadaAppPythonQtDecorators : public QObject {
  Q_OBJECT
//
public:
  explicit qColadaAppPythonQtDecorators(QObject *parent = nullptr):
    QObject(parent) {

    // `ctkCheckableHeaderView` is doesn't have a default constructor and
    // to fix that I add it in decorator. See more: https://github.com/commontk/CTK/pull/985
    PythonQt::self()->registerClass(
          &ctkCheckableHeaderView::staticMetaObject, "qColadaApp");


//    auto m = PythonQt::self()->importModule("h5gtpy"); // gives error as it doesn't have default constructor?
//    PythonQt::self()->registerCPPClass(
//          "h5gt::File", "", "qColadaApp");

//    PythonQt::self()->importModule("h5geopy");
//    PythonQt::self()->registerCPPClass(
//          "H5SeisContainer", "", "h5geopy");

    PythonQt::self()->registerCPPClass(
          "DBCore", "", "qColadaApp");
    PythonQt::self()->registerCPPClass(
          "Util", "", "qColadaApp");
  }

public slots:
  ctkCheckableHeaderView *new_ctkCheckableHeaderView(Qt::Orientation orient, QWidget *parent = nullptr) {
    return new ctkCheckableHeaderView(orient, parent);
  }

  Util *new_Util(){
    return new Util();
  }

  DBCore *new_DBCore(){
    return new DBCore();
  }

//  h5gt::File *new_File(std::string fileName, int val) {
//    return new h5gt::File(fileName, val);
//  }

  /*---------ADD STATIC METHODS TO `DBCore` class---------*/
  QSqlDatabase static_DBCore_createDB(QString &fullName){ return dbcore::createDB(fullName); }
  QSqlDatabase static_DBCore_createDB(const QString &path, QString &name){ return dbcore::createDB(path, name); }
  bool static_DBCore_createInfoTable(){ return dbcore::createInfoTable(); }
  bool static_DBCore_fillInfoTable(
      const QString &owner, const QString &prjName,
      const QString &prjDir, const QString &units,
      const QString &crsAuthName, const QString &crsCode,
      const QString &crsName)
  { return dbcore::fillInfoTable(owner, prjName, prjDir, units, crsAuthName, crsCode, crsName); }
  bool static_DBCore_createSeisTable(){ return dbcore::createSeisTable(); }
  bool static_DBCore_createWellTable(){ return dbcore::createWellTable(); }
  bool static_DBCore_createMapTable(){ return dbcore::createMapTable(); }
  bool static_DBCore_createProjectFolders(const QString &prjDir){ return dbcore::createProjectFolders(prjDir); }
  QString static_DBCore_getCurrentProjectName(){ return dbcore::getCurrentProjectName(); }
  QString static_DBCore_getCurrentProjectUnits(){ return dbcore::getCurrentProjectUnits(); }
  QString static_DBCore_getCurrentProjectDir(){ return dbcore::getCurrentProjectDir(); }
  QString static_DBCore_getSeisDir(){ return dbcore::getSeisDir(); }
  QString static_DBCore_getMapDir(){ return dbcore::getMapDir(); }
  QString static_DBCore_getWellDir(){ return dbcore::getWellDir(); }
  QSqlQuery static_DBCore_getTable(const QString &tableName){ return dbcore::getTable(tableName); }
  QSqlQuery static_DBCore_getFromTable(
      const QString &tableName, const QString &colName, const QString &value)
  { return dbcore::getFromTable(tableName, colName, value); }
  QVariantList static_DBCore_getAvailableProjections(){
    QStringList authNameList, codeList, nameList;
    bool val = dbcore::getAvailableProjections(authNameList, codeList, nameList);
    return QVariantList({authNameList, codeList, nameList, val});
  }
  QVariantList static_DBCore_getCurrentProjectionInfo(){
    QString authName, code, name;
    dbcore::getCurrentProjection(authName, code, name);
    return QVariantList({authName, code, name});
  }
  OGRSpatialReference static_DBCore_getCurrentProjection(){ return dbcore::getCurrentProjection(); }
  QString static_DBCore_getCurrentProjectionNameCode(){ return dbcore::getCurrentProjectionNameCode(); }
  QVariantList static_DBCore_convCoord2CurrentProjection(
      double x, double y, QByteArray authCodeFrom, const QString &units)
  {
    bool val = dbcore::convCoord2CurrentProjection(1, &x, &y, authCodeFrom, units);
    return QVariantList({x, y, val});
  }
  QVariantList static_DBCore_convCoord2CurrentProjection(
      std::vector<double>& x_vec,
      std::vector<double>& y_vec,
      QByteArray authCodeFrom,
      const QString &units)
  {
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

  bool static_DBCore_initProjLibDB(){
    return dbcore::initProjLibDB();
  }


  /*---------ADD STATIC METHODS TO `Util` class---------*/
  QRegularExpression static_Util_fileNameRegExp(){ return util::fileNameRegExp(); }
  QRegularExpression static_Util_fileNameNoSpaceRegExp(){ return util::fileNameNoSpaceRegExp(); }
  QRegExp static_Util_floatRegExp(){ return util::floatRegExp(); }
  QString fstatic_Util_ileNameToolTipText(){ return util::fileNameToolTipText(); }
  QString static_Util_fileNameNoSpaceToolTipText(){ return util::fileNameNoSpaceToolTipText(); }
  QStringList static_Util_readTxtFileByLines(const QString &filePath){ return util::readTxtFileByLines(filePath); }
  bool rstatic_Util_emoveDir(const QString &dirName){ return util::removeDir(dirName); }
  double static_Util_convertUnits(
      const QString &unitsFrom, const QString &unitsTo){
    return units::convert(
          units::unit_from_string(unitsFrom.toStdString()),
          units::unit_from_string(unitsTo.toStdString()));
  }


  // TEST EXTERNAL LIBRARY COMPLIENCE
//  std::string static_Util_h5gt(h5gt::File file){
//    return file.getFileName();
//  }
//  std::string static_Util_h5geo(H5SeisContainer* seisCnt){
//    return seisCnt->getH5File().getFileName();
//  }
};

Q_DECLARE_METATYPE(std::vector<double>)



void initqColadaAppPythonQtDecorators() {
  PythonQt::self()->addDecorators(new qColadaAppPythonQtDecorators);
}

#endif
