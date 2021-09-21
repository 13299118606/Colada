//#ifndef __qColadaDevReader_h
//#define __qColadaDevReader_h
//
//// Colada includes
//#include "qColadaReader.h"
//#include "DevRead.h"
//
//class qColadaDevReaderPrivate;
//class QString;
//
//class Q_COLADA_APP_EXPORT qColadaDevReader : public qColadaReader {
//  Q_OBJECT
//
//public:
//  explicit qColadaDevReader(QWidget *parent = nullptr);
//  virtual ~qColadaDevReader() override;
//
//public slots:
//  virtual void updateRow(int row) override;
//  virtual void resetRow(int row) override;
//
//  virtual void onAddBtnClicked() override;
//  virtual void onButtonBoxClicked(QAbstractButton *button) override;
//
//private:
//  DevRead::Param getParamFromTable(int row, QString &errMsg);
//
//private:
//  Q_DECLARE_PRIVATE(qColadaDevReader);
//  Q_DISABLE_COPY(qColadaDevReader);
//};
//
//#endif // __qColadaDevReader_h
