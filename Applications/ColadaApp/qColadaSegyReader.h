#ifndef __qColadaSegyReader_h
#define __qColadaSegyReader_h

// Colada includes
#include "qColadaReader.h"
#include "SegyRead.h"

class qColadaSegyReaderPrivate;
class QString;

class Q_COLADA_APP_EXPORT qColadaSegyReader : public qColadaReader {
  Q_OBJECT

public:
  explicit qColadaSegyReader(QWidget *parent = nullptr);
  virtual ~qColadaSegyReader() override;

public slots:
  virtual void updateRow(int proxy_row) override;
  virtual void resetRow(int proxy_row) override;
  void addBytesTableRow(const QString& readFile);
  void removeBytesTableRow(const QString& readFile);

  virtual void onAddBtnClicked() override;
  virtual void onRemoveToolBtnClicked() override;
  virtual void onAutoDefineToolBtnClicked() override;
  virtual void onButtonBoxClicked(QAbstractButton *button) override;
  void onBytesTableDataChanged(
      const QModelIndex &topLeft,
      const QModelIndex &bottomRight,
      const QVector<int> &roles = QVector<int>());

private:
  SegyRead::ReadOnlyParam getReadOnlyParamFromTable(
      int proxy_row,
      QString &errMsg);
  SegyRead::ReadWriteParam getReadWriteParamFromTable(
      int proxy_row,
      QString &errMsg);
  std::vector<std::string> getTrcHdrNamesFromBytesTable(
      const QString& readFile);

private:
  Q_DECLARE_PRIVATE(qColadaSegyReader);
  Q_DISABLE_COPY(qColadaSegyReader);
};

#endif // __qColadaSegyReader_h
