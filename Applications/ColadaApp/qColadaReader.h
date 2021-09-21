#ifndef __qColadaReader_h
#define __qColadaReader_h

// Qt includes
#include <QDialog>

// Colada includes
#include "qColadaAppExport.h"

class qColadaReaderPrivate;
class QAbstractButton;

class Q_COLADA_APP_EXPORT qColadaReader : public QDialog {
  Q_OBJECT

public:
  explicit qColadaReader(QWidget *parent = nullptr);
  virtual ~qColadaReader() override;

public slots:
  virtual void updateRow(int row);
  virtual void resetRow(int row);

  virtual void onAddBtnClicked();
  virtual void onRemoveToolBtnClicked();
  virtual void onAutoDefineToolBtnClicked();
  virtual void onButtonBoxClicked(QAbstractButton *button);

protected:
  QScopedPointer<qColadaReaderPrivate> d_ptr;

  explicit qColadaReader(qColadaReaderPrivate *pimpl, QWidget *parent);


private:
  Q_DECLARE_PRIVATE(qColadaReader);
  Q_DISABLE_COPY(qColadaReader);
};

#endif // __qColadaReader_h
