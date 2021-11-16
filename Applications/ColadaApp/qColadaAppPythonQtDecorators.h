#ifndef __qColadaAppPythonQtDecorators_h
#define __qColadaAppPythonQtDecorators_h

#include "PythonQt.h"
#include "PythonQtCppWrapperFactory.h"

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkCheckableHeaderView.h>

// Colada includes
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


class Util {};

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
          "Util", "", "qColadaApp");
  }

public slots:
  ctkCheckableHeaderView *new_ctkCheckableHeaderView(Qt::Orientation orient, QWidget *parent = nullptr) {
    return new ctkCheckableHeaderView(orient, parent);
  }

  Util *new_Util(){
    return new Util();
  }

//  h5gt::File *new_File(std::string fileName, int val) {
//    return new h5gt::File(fileName, val);
//  }

  /*---------ADD STATIC METHODS TO `Util` class---------*/
  QRegularExpression static_Util_fileNameRegExp(){ return util::fileNameRegExp(); }
  QRegularExpression static_Util_fileNameNoSpaceRegExp(){ return util::fileNameNoSpaceRegExp(); }
  QRegExp static_Util_floatRegExp(){ return util::floatRegExp(); }
  QString static_Util_fileNameToolTipText(){ return util::fileNameToolTipText(); }
  QString static_Util_fileNameNoSpaceToolTipText(){ return util::fileNameNoSpaceToolTipText(); }
  QStringList static_Util_readTxtFileByLines(const QString &filePath){ return util::readTxtFileByLines(filePath); }
  bool static_Util_removeDir(const QString &dirName){ return util::removeDir(dirName); }
  QString static_Util_CRSName(){ return util::CRSName(); };
  QString static_Util_CRSAuthName(){ return util::CRSAuthName(); };
  int static_Util_CRSCode(){ return util::CRSCode(); };

  QString static_Util_lengthUnits(){ return util::lengthUnits(); };
  QString static_Util_timeUnits(){ return util::timeUnits(); };
  QString static_Util_frequencyUnits(){ return util::frequencyUnits(); };
  QString static_Util_velocityUnits(){ return util::velocityUnits(); };
  QString static_Util_intensityUnits(){ return util::intensityUnits(); };
  QString static_Util_areaUnits(){ return util::areaUnits(); };
  QString static_Util_volumeUnits(){ return util::volumeUnits(); };
  QString static_Util_defaultSeisDir(){ return util::defaultSeisDir(); };
  QString static_Util_defaultWellDir(){ return util::defaultWellDir(); };
  QString static_Util_defaultMapDir(){ return util::defaultMapDir(); };

  bool static_Util_getAvailableProjections(
      QStringList &authNameList,
      QStringList &codeList,
      QStringList &nameList){
    return util::getAvailableProjections(
          authNameList, codeList, nameList);
  };
  void static_Util_getCurrentProjection(
      QString &authName,
      QString &code,
      QString &name){
    return util::getCurrentProjection(authName, code, name);
  };
  OGRSpatialReference static_Util_getCurrentProjection(){ return util::getCurrentProjection(); };

  bool static_Util_convCoord2CurrentProjection(
      int nCount, double *x,
      double *y,
      QByteArray authCodeFrom,
      const QString &units){
    return util::convCoord2CurrentProjection(
          nCount, x, y, authCodeFrom, units);
  };

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
