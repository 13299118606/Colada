#ifndef __util_h
#define __util_h

// std includes
#include <cstring>  // memcpy
#include <type_traits>
#include <vector>

// Qt includes

class QRegularExpression;
class QRegExp;
class QString;
class QStringList;
class OGRSpatialReference;
class QByteArray;

// PythonQt wrapper for functions is in `qColadaAppPythonQtDecorators.h`

namespace util {

QRegularExpression fileNameRegExp();
QRegularExpression fileNameNoSpaceRegExp();
QRegExp floatRegExp();

QString fileNameToolTipText();
QString fileNameNoSpaceToolTipText();

QStringList readTxtFileByLines(const QString &filePath);

///
/// \brief removeDir removes content of a dir. Returns 0 if fails
/// \param dirName
/// \return
///
bool removeDir(const QString &dirName);

QString CRSName();
QString CRSAuthName();
int CRSCode();

QString LengthUnits();
QString TimeUnits();
QString FrequencyUnits();
QString VelocityUnits();
QString IntensityUnits();
QString AreaUnits();
QString VolumeUnits();

bool getAvailableProjections(
    QStringList &authNameList,
    QStringList &codeList,
    QStringList &nameList);

void getCurrentProjection(
    QString &authName,
    QString &code,
    QString &name);

OGRSpatialReference getCurrentProjection();

bool convCoord2CurrentProjection(
    int nCount,
    double *x,
    double *y,
    QByteArray authCodeFrom,
    const QString &units);

bool initProjLibDB();


} // namespace util


#endif
