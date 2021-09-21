#ifndef __ColadaUtil_py_h
#define __ColadaUtil_py_h

// Colada includes
#include "qColadaAppExport.h"

// Qt includes - need to be included for MOC
#include <QObject>
#include <QRegularExpression>

class Q_COLADA_APP_EXPORT ColadaUtil_py : public QObject {
  Q_OBJECT

public:
  ColadaUtil_py(QObject *parent = nullptr);
  ~ColadaUtil_py() = default;

public slots:
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

  double convertUnits(const QString &unitsFrom, const QString &unitsTo);

};

#endif