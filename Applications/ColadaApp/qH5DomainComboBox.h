#ifndef __qH5DomainComboBox_h
#define __qH5DomainComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5DomainComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5DomainComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5DomainComboBox(QWidget *parent = nullptr);
  virtual ~qH5DomainComboBox() override;

public slots:
  bool setH5Domain(unsigned domain);
  unsigned getH5Domain();

protected:
  QScopedPointer<qH5DomainComboBoxPrivate> d_ptr;

  explicit qH5DomainComboBox(qH5DomainComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5DomainComboBox);
  Q_DISABLE_COPY(qH5DomainComboBox);
};

#endif
