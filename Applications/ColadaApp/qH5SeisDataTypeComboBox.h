#ifndef __qH5SeisDataTypeComboBox_h
#define __qH5SeisDataTypeComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5SeisDataTypeComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5SeisDataTypeComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5SeisDataTypeComboBox(QWidget *parent = nullptr);
  virtual ~qH5SeisDataTypeComboBox() override;

public slots:
  unsigned getH5SeisDataType();

protected:
  QScopedPointer<qH5SeisDataTypeComboBoxPrivate> d_ptr;

  explicit qH5SeisDataTypeComboBox(qH5SeisDataTypeComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5SeisDataTypeComboBox);
  Q_DISABLE_COPY(qH5SeisDataTypeComboBox);
};

#endif
