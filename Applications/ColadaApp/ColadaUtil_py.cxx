// Colada includes
#include "ColadaUtil_py.h"
#include "ColadaUtil.h"

// units includes
#include <units/units.hpp>

ColadaUtil_py::ColadaUtil_py(QObject *parent) : QObject(parent) {}

QRegularExpression ColadaUtil_py::fileNameRegExp() {
  return util::fileNameRegExp();
}

QRegularExpression ColadaUtil_py::fileNameNoSpaceRegExp() {
  return util::fileNameNoSpaceRegExp();
}

QRegExp ColadaUtil_py::floatRegExp() { 
  return util::floatRegExp(); 
}

QString ColadaUtil_py::fileNameToolTipText() {
  return util::fileNameToolTipText();
}

QString ColadaUtil_py::fileNameNoSpaceToolTipText() {
  return util::fileNameNoSpaceToolTipText();
}

QStringList ColadaUtil_py::readTxtFileByLines(const QString &filePath) {
  return util::readTxtFileByLines(filePath);
}

bool ColadaUtil_py::removeDir(const QString &dirName) {
  return util::removeDir(dirName);
}

double ColadaUtil_py::convertUnits(const QString &unitsFrom,
                                   const QString &unitsTo) {
  return units::convert(units::unit_from_string(unitsFrom.toStdString()),
                        units::unit_from_string(unitsTo.toStdString()));
}
