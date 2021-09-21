//#include "DevRead.h"
//
//#include "ColadaUtil.h"
//
//#include <QFile>
//
//DevRead::DevRead(QString readFile, h5geo::Delimiter delimiter)
//    : readFile(readFile), delimiter(delimiter) {
//  colNameMap["X"] = -1;
//  colNameMap["Y"] = -1;
//  colNameMap["Z"] = -1;
//  colNameMap["MD"] = -1;
//  colNameMap["TVD"] = -1;
//  colNameMap["TVDSS"] = -1;
//  colNameMap["DX"] = -1;
//  colNameMap["DY"] = -1;
//  colNameMap["AZIM"] = -1;
//  colNameMap["INCL"] = -1;
//
//  init();
//  initColNameMap();
//}
//
//DevRead::~DevRead() {}
//
//void DevRead::init() {
//  char delimiterChar = h5geo::getDelimiter(delimiter);
//
//  QFile qFile(readFile);
//  if (!qFile.open(QFile::ReadOnly | QFile::Text))
//    return;
//
//  QRegExp floatRE = util::floatRegExp();
//
//  QTextStream in(&qFile);
//  while (!in.atEnd()) {
//    QString line = in.readLine();
//    QStringList lineList = line.split(delimiterChar, Qt::SkipEmptyParts);
//    int counter = 0;
//    for (QString &str : lineList) {
//      counter += floatRE.exactMatch(str);
//    }
//
//    if (counter == lineList.count())
//      break;
//
//    headerList.push_back(line);
//  }
//  qFile.close();
//}
//
//void DevRead::initColNameMap() {
//  char delimiterChar = h5geo::getDelimiter(delimiter);
//  QStringList keys = colNameMap.keys();
//  for (QString &line : headerList) {
//    QStringList lineList =
//        line.split(delimiterChar, Qt::SkipEmptyParts, Qt::CaseInsensitive);
//    int counter = 0;
//    for (QString &str : lineList)
//      counter += keys.contains(str, Qt::CaseInsensitive);
//
//    if (counter == lineList.count()) {
//      colNameList = lineList;
//      return;
//    }
//  }
//}
//
//QString DevRead::getWellName() {
//  for (QString &line : headerList) {
//    QString wellName =
//        line.section(wellNameSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//            .trimmed();
//    if (!wellName.isEmpty()) {
//      QRegularExpression re = util::fileNameRegExp();
//      QRegularExpressionMatch match = re.match(wellName);
//      return match.capturedTexts().join("");
//    }
//  }
//  return QString();
//}
//
//QString DevRead::getUWI() {
//  for (QString &line : headerList) {
//    QString uwi =
//        line.section(uwiSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//            .trimmed();
//    if (!uwi.isEmpty()) {
//      QRegularExpression re = util::fileNameRegExp();
//      QRegularExpressionMatch match = re.match(uwi);
//      return match.capturedTexts().join("");
//    }
//  }
//  return QString();
//}
//
//QString DevRead::getWellType() {
//  for (QString &line : headerList) {
//    QString wellType =
//        line.section(wellTypeSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//            .trimmed();
//    if (!wellType.isEmpty()) {
//      QMap<QString, h5geo::WellType> wellTypeMap = h5geo::mapWellType();
//      QMapIterator<QString, h5geo::WellType> i(wellTypeMap);
//      while (i.hasNext()) {
//        i.next();
//        if (QString::compare(i.key(), wellType, Qt::CaseInsensitive) == 0)
//          return i.key();
//      }
//    }
//  }
//  return QString();
//}
//
//double DevRead::getKB() {
//  for (QString &line : headerList) {
//    QString kb = line.section(kbSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//                     .trimmed();
//    if (!kb.isEmpty())
//      return kb.toDouble();
//  }
//  return qQNaN();
//}
//
//double DevRead::getHeadXCoord() {
//  for (QString &line : headerList) {
//    QString headX =
//        line.section(headXSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//            .trimmed();
//    if (!headX.isEmpty())
//      return headX.toDouble();
//  }
//  return qQNaN();
//}
//
//double DevRead::getHeadYCoord() {
//  for (QString &line : headerList) {
//    QString headY =
//        line.section(headYSep, 1, -1, QString::SectionCaseInsensitiveSeps)
//            .trimmed();
//    if (!headY.isEmpty())
//      return headY.toDouble();
//  }
//  return qQNaN();
//}
//
//double DevRead::getSkipLines() { return headerList.count(); }
//
//int DevRead::getColNum(QString colName) {
//  if (colNameList.isEmpty())
//    return -1;
//
//  return colNameList.indexOf(colName);
//}
//
//arma::mat DevRead::getDATA(QString readFile, arma::uvec cols,
//                           h5geo::Delimiter delimiter, int skipLines) {
//  QFile qFile(readFile);
//  if (!qFile.open(QFile::ReadOnly | QFile::Text))
//    return arma::mat();
//
//  QRegExp floatRE = util::floatRegExp();
//  char delimiterChar = h5geo::getDelimiter(delimiter);
//
//  int lineNum = 0;
//  arma::mat DATA;
//  QTextStream in(&qFile);
//  while (!in.atEnd()) {
//    QString line = in.readLine();
//    if (lineNum >= skipLines) {
//      QStringList lineList = line.split(delimiterChar, Qt::SkipEmptyParts);
//      int counter = 0;
//      for (QString &str : lineList)
//        counter += floatRE.exactMatch(str);
//
//      if (counter != lineList.count() || lineList.count() < cols.max())
//        return DATA;
//
//      DATA.resize(DATA.n_rows + 1, cols.n_elem);
//      for (size_t j = 0; j < cols.size(); j++)
//        DATA(DATA.n_rows - 1, j) = lineList[cols(j)].toDouble();
//    }
//    lineNum++;
//  }
//  qFile.close();
//  return DATA;
//}
//
//std::optional<DevCurve> DevRead::read(Param &p, QString &errMsg) {
//  if (p.readFile.isEmpty()) {
//    errMsg = "Empty .dev file name";
//    return std::nullopt;
//  }
//  if (p.saveFile.isEmpty()) {
//    errMsg = p.readFile + ": Empty .h5 file name";
//    return std::nullopt;
//  }
//  if (p.delimiter == static_cast<h5geo::Delimiter>(0)) {
//    errMsg = p.readFile + ": Invalid delimiter";
//    return std::nullopt;
//  }
//  if (p.wellCreateType == static_cast<h5geo::CreationType>(0)) {
//    errMsg = p.readFile + ": Invalid well create type";
//    return std::nullopt;
//  }
//  if (p.devCreateType == static_cast<h5geo::CreationType>(0)) {
//    errMsg = p.readFile + ": Invalid deviation create type";
//    return std::nullopt;
//  }
//  if (p.wellName.isEmpty()) {
//    errMsg = p.readFile + ": Invalid well name";
//    return std::nullopt;
//  }
//  if (p.skipLines < 0) {
//    errMsg = p.readFile + ": Invalid skip lines number";
//    return std::nullopt;
//  }
//  if (p.depthMult != -1 && p.depthMult != 1) {
//    errMsg = p.readFile + ": Invalid depth multiplier number";
//    return std::nullopt;
//  }
//  if (p.angleUnits == static_cast<h5geo::AngleUnits>(0)) {
//    errMsg = p.readFile + ": Invalid angle units";
//    return std::nullopt;
//  }
//  if (p.spatialUnits == static_cast<h5geo::SpatialUnits>(0)) {
//    errMsg = p.readFile + ": Invalid spatial units";
//    return std::nullopt;
//  }
//  if (p.trajFormat == static_cast<h5geo::TrajectoryFormat>(0)) {
//    errMsg = p.readFile + ": Invalid trajectory format";
//    return std::nullopt;
//  }
//
//  std::optional<WellContainer> wellContainer =
//      WellContainer::create(p.saveFile);
//  if (!wellContainer.has_value()) {
//    errMsg = p.readFile + ": Can't open or create well container";
//    return std::nullopt;
//  }
//
//  std::optional<Well> well =
//      wellContainer->createWell(p.wellName, p.wellCreateType);
//  if (!well.has_value()) {
//    errMsg = p.readFile + ": Can't open or create well";
//    return std::nullopt;
//  }
//
//  if (!qIsNaN(p.headX))
//    well->setHeadX(p.headX);
//  if (!qIsNaN(p.headY))
//    well->setHeadY(p.headY);
//  if (!qIsNaN(p.kb))
//    well->setKB(p.kb);
//  if (!p.uwi.isEmpty())
//    well->setUWI(p.uwi);
//
//  std::optional<DevCurve> devCurve =
//      well->createDevCurve(p.devCurveName, 1000, p.devCreateType);
//  if (!devCurve.has_value()) {
//    errMsg = p.readFile + ": Can't open or create deviation curve";
//    return std::nullopt;
//  }
//
//  arma::uvec cols({p.coordCol1, p.coordCol2, p.coordCol3});
//
//  arma::mat DATA = getDATA(p.readFile, cols, p.delimiter, p.skipLines);
//  if (DATA.is_empty()) {
//    errMsg = p.readFile + ": Can't read depth/coordinate columns";
//    return std::nullopt;
//  }
//
//  if (p.trajFormat == h5geo::TrajectoryFormat::MD_AZIM_INCL) {
//    if (p.spatialUnits == h5geo::SpatialUnits::FOOT) {
//      DATA.col(0) = DATA.col(0) * 0.3048;
//    }
//    if (p.angleUnits == h5geo::AngleUnits::DEGREE) {
//      DATA.col(1) = DATA.col(1) * arma::datum::pi / 180;
//      DATA.col(2) = DATA.col(2) * arma::datum::pi / 180;
//    }
//  } else {
//    if (p.spatialUnits == h5geo::SpatialUnits::FOOT) {
//      DATA = DATA * 0.3048;
//    }
//  }
//
//  arma::mat DATA_MdXYTvd;
//  if (p.trajFormat == h5geo::TrajectoryFormat::MD_AZIM_INCL) {
//    DATA_MdXYTvd = Deviation::MdAzIncl2MdXYTvd(DATA, p.headX, p.headY);
//  } else if (p.trajFormat == h5geo::TrajectoryFormat::TVD_X_Y) {
//    DATA_MdXYTvd = Deviation::TvdXY2MdXYTvd(DATA, p.headX, p.headY);
//  } else if (p.trajFormat == h5geo::TrajectoryFormat::TVDSS_X_Y) {
//    DATA_MdXYTvd = Deviation::TvdssXY2MdXYTvd(DATA, p.headX, p.headY, p.kb);
//  } else if (p.trajFormat == h5geo::TrajectoryFormat::TVD_DX_DY) {
//    DATA_MdXYTvd = Deviation::TvdDxDy2MdXYTvd(DATA, p.headX, p.headY);
//  } else if (p.trajFormat == h5geo::TrajectoryFormat::TVDSS_DX_DY) {
//    DATA_MdXYTvd = Deviation::TvdssDxDy2MdXYTvd(DATA, p.headX, p.headY, p.kb);
//  }
//
//  if (DATA_MdXYTvd.is_empty()) {
//    errMsg = p.readFile + ": Can't convert data to MD_X_Y_TVD";
//    return std::nullopt;
//  }
//
//  dbcore::convCoord2CurrentProjection(p.headX, p.headY, p.crs.toLocal8Bit());
//  dbcore::convCoord2CurrentProjection(DATA_MdXYTvd, 1, 2, p.crs.toLocal8Bit());
//
//  DATA_MdXYTvd.col(3) = DATA_MdXYTvd.col(3) * p.depthMult;
//
//  arma::vec data = DATA_MdXYTvd.col(0);
//  devCurve->writeMD(data);
//
//  data = DATA_MdXYTvd.col(1);
//  devCurve->writeX(data);
//
//  data = DATA_MdXYTvd.col(2);
//  devCurve->writeY(data);
//
//  data = DATA_MdXYTvd.col(3);
//  devCurve->writeTVD(data);
//
//  return devCurve;
//}
