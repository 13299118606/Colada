#ifndef __qH5ContainerTypeComboBox_h
#define __qH5ContainerTypeComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5ContainerTypeComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5ContainerTypeComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5ContainerTypeComboBox(QWidget *parent = nullptr);
  virtual ~qH5ContainerTypeComboBox() override;

public slots:
  bool setH5ContainerType(unsigned containerType);
  unsigned getH5ContainerType();

protected:
  QScopedPointer<qH5ContainerTypeComboBoxPrivate> d_ptr;

  explicit qH5ContainerTypeComboBox(qH5ContainerTypeComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5ContainerTypeComboBox);
  Q_DISABLE_COPY(qH5ContainerTypeComboBox);
};

#endif
