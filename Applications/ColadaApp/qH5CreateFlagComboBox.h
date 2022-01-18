#ifndef __qH5CreateFlagComboBox_h
#define __qH5CreateFlagComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5CreateFlagComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5CreateFlagComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5CreateFlagComboBox(QWidget *parent = nullptr);
  virtual ~qH5CreateFlagComboBox() override;

public slots:
  void addH5Flag(unsigned flag);
  void removeH5Flag(unsigned flag);

  /// OPEN, OPEN_OR_CREATE
  void setH5FlagsToOpen();
  /// CREATE, CREATE_OR_OVERWRITE, CREATE_UNDER_NEW_NAME
  void setH5FlagsToCreate();
  /// OPEN, CREATE, OPEN_OR_CREATE, CREATE_OR_OVERWRITE, CREATE_UNDER_NEW_NAME
  void setH5FlagsToAll();
  /// retun h5geo::CreationType as unsigned value
  unsigned getH5CreateFlag();

protected:
  QScopedPointer<qH5CreateFlagComboBoxPrivate> d_ptr;

  explicit qH5CreateFlagComboBox(qH5CreateFlagComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5CreateFlagComboBox);
  Q_DISABLE_COPY(qH5CreateFlagComboBox);
};

#endif
