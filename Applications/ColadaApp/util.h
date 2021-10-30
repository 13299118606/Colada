#ifndef __util_h
#define __util_h

// std includes
#include <cstring>  // memcpy
#include <type_traits>
#include <vector>

// Qt includes
#include <QString>
#include <QTableView>

// magic_enum includes
#include <magic_enum.hpp>

// Colada includes
#include "qComboBoxDelegate.h"
class QRegularExpression;
class QRegExp;
class QString;
class QStringList;

// PythonQt wrapper for functions is in `qColadaAppPythonQtDecorators.h`

namespace util {

QRegularExpression fileNameRegExp();
QRegularExpression fileNameNoSpaceRegExp();
QRegExp floatRegExp();

QString fileNameToolTipText();
QString fileNameNoSpaceToolTipText();

QStringList readTxtFileByLines(const QString &filePath);
QStringList toQStringList(const std::vector<std::string> &stdNameList);

///
/// \brief removeDir removes content of a dir. Returns 0 if fails
/// \param dirName
/// \return
///
bool removeDir(const QString &dirName);

template <class To, class From>
typename std::enable_if<(sizeof(To) == sizeof(From)) &&
                            std::is_trivially_copyable<From>::value &&
                            std::is_trivial<To>::value,
                        // this implementation requires that To is
                        // trivially default constructible
                        To>::type
    // constexpr support needs compiler magic
    inline bit_cast(const From &src) noexcept {
  To dst;
  std::memcpy(&dst, &src, sizeof(To));
  return dst;
}

template <typename T>
/// /brief Type `T` is specified as enum class. All enum class
/// members are added to combo.
/// /tparam T
/// /param tableView
/// /param
/// /return
void setComboDelegateFromEnumClassForTable(QTableView *tableView, int col) {
  auto comboData = magic_enum::enum_names<T>();
  QStringList comboDataList;
  for (const auto &name : comboData) {
    comboDataList.push_back(QString::fromStdString(std::string(name)));
  }
  qComboBoxDelegate *comboDelegate = new qComboBoxDelegate(comboDataList);
  comboDelegate->setParent(tableView);
  tableView->setItemDelegateForColumn(col, comboDelegate);
}

} // namespace util


#endif
