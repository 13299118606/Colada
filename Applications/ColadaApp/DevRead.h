//#ifndef __DevRead_h
//#define __DevRead_h
//
//// h5geo includes
//#include "h5geo/h5core.h"
//#include "h5geo/h5well.h"
//#include "h5geo/h5wellcontainer.h"
//
//// Eigen includes
//#include <Eigen/Dense>
//
//class DevRead {
//public:
//  struct Param {
//    QString readFile, saveFile, crs, devCurveName, wellName, uwi;
//    h5geo::TrajectoryFormat trajFormat;
//    h5geo::WellType wellType;
//    h5geo::CreationType wellCreateType, devCreateType;
//    h5geo::SpatialUnits spatialUnits;
//    h5geo::AngleUnits angleUnits;
//    h5geo::Delimiter delimiter;
//    double kb, headX, headY;
//    int depthMult;
//    size_t skipLines, coordCol1, coordCol2, coordCol3;
//  };
//
//  explicit DevRead(QString readFile,
//                   h5geo::Delimiter delimiter = h5geo::Delimiter::SPACE);
//  virtual ~DevRead();
//
//  QString getWellName();
//  QString getUWI();
//  QString getWellType();
//  double getKB();
//  double getHeadXCoord();
//  double getHeadYCoord();
//  double getSkipLines();
//  int getColNum(QString colName);
//
//  static Eigen::MatrixXd getDATA(QString readFile, arma::uvec cols,
//                           h5geo::Delimiter delimiter, int skipLines);
//  static std::optional<DevCurve> read(Param &p, QString &errMsg);
//
//private:
//  void init();
//  void initColNameMap();
//
//public:
//  QString readFile;
//  QStringList headerList;
//  h5core::Delimiter delimiter;
//  QString wellNameSep = "WELL NAME:";
//  QString uwiSep = "UWI:";
//  QString wellTypeSep = "WELL TYPE:";
//  QString kbSep = "WELL KB:";
//  QString headXSep = "WELL HEAD X-COORDINATE:";
//  QString headYSep = "WELL HEAD Y-COORDINATE:";
//  QMap<QString, int> colNameMap;
//  QStringList colNameList;
//};
//
//#endif // DevReader_h
