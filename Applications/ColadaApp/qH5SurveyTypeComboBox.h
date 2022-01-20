#ifndef __qH5SurveyTypeComboBox_h
#define __qH5SurveyTypeComboBox_h

// Qt includes
#include <QComboBox>

// Colada includes
#include "qColadaAppExport.h"

class qH5SurveyTypeComboBoxPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qH5SurveyTypeComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit qH5SurveyTypeComboBox(QWidget *parent = nullptr);
  virtual ~qH5SurveyTypeComboBox() override;

public slots:
  bool setH5SurveyType(unsigned survType);
  unsigned getH5SurveyType();

protected:
  QScopedPointer<qH5SurveyTypeComboBoxPrivate> d_ptr;

  explicit qH5SurveyTypeComboBox(qH5SurveyTypeComboBoxPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qH5SurveyTypeComboBox);
  Q_DISABLE_COPY(qH5SurveyTypeComboBox);
};

#endif
