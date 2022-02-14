#ifndef __qH5ObjectTypeComboBox_h
#define __qH5ObjectTypeComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5ObjectTypeComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5ObjectTypeComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5ObjectTypeComboBox(QWidget *parent = nullptr);
  virtual ~qH5ObjectTypeComboBox() override;

public slots:
  bool setH5ObjectType(unsigned objectType);
  unsigned getH5ObjectType();

protected:
  QScopedPointer<qH5ObjectTypeComboBoxPrivate> d_ptr;

  explicit qH5ObjectTypeComboBox(qH5ObjectTypeComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5ObjectTypeComboBox);
  Q_DISABLE_COPY(qH5ObjectTypeComboBox);
};

#endif
