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

  virtual void onAddBtnClicked() override;
  virtual void onRemoveToolBtnClicked() override;
  virtual void onAutoDefineToolBtnClicked() override;
  virtual void onButtonBoxClicked(QAbstractButton *button) override;

private:
  SegyRead::ReadOnlyParam getReadOnlyParamFromTable(int proxy_row,
                                                    QString &errMsg);
  SegyRead::ReadWriteParam getReadWriteParamFromTable(int proxy_row,
                                                      QString &errMsg);

private:
  Q_DECLARE_PRIVATE(qColadaSegyReader);
  Q_DISABLE_COPY(qColadaSegyReader);
};

#endif // __qColadaSegyReader_h