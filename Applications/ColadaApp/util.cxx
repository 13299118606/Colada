#include "util.h"

// Qt includes
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>

// h5geo includes
#include <h5geo/h5core.h>

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

QStringList util::toQStringList(const std::vector<std::string>& stdNameList) {
	QStringList nameList;
	nameList.reserve(stdNameList.size());
	for (int i = 0; i < stdNameList.size(); i++)
		nameList.append(QString::fromStdString(stdNameList[i]));

	return nameList;
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
