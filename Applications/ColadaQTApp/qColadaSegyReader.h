#ifndef __qColadaSegyReader_h
#define __qColadaSegyReader_h

// Colada includes
#include "qColadaReader.h"

class qColadaSegyReaderPrivate;

class Q_COLADA_APP_EXPORT qColadaSegyReader : public qColadaReader {
  Q_OBJECT

public:
  explicit qColadaSegyReader(QWidget *parent = nullptr);
  virtual ~qColadaSegyReader() override;

  virtual void updateRow(int row) override;
  virtual void resetRow(int row) override;

public slots:
  virtual void onAddBtnClicked() override;
  virtual void onButtonBoxClicked(QAbstractButton *button) override;

private:
  Q_DECLARE_PRIVATE(qColadaSegyReader);
  Q_DISABLE_COPY(qColadaSegyReader);
};

#endif // __qColadaSegyReader_h
