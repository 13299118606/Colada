#ifndef __qScienceSpinBox_h
#define __qScienceSpinBox_h

#include <QDoubleSpinBox>

// Colada includes
#include "qColadaAppExport.h"

class qScienceSpinBoxPrivate;

bool isIntermediateValueHelper(qint64 num, qint64 minimum,
                               qint64 maximum, qint64 *match = 0);

class Q_COLADA_APP_EXPORT qScienceSpinBox : public QDoubleSpinBox {
  Q_OBJECT

public:
  qScienceSpinBox(QWidget *parent = nullptr);
  virtual ~qScienceSpinBox() override;

  int decimals() const;
  void setDecimals(int value);

  QString textFromValue(double value) const;
  double valueFromText(const QString &text) const;

private:
  bool isIntermediateValue(const QString &str) const;
  QVariant validateAndInterpret(QString &input, int &pos,
                                QValidator::State &state) const;
  QValidator::State validate(QString &text, int &pos) const;
  void fixup(QString &input) const;
  QString stripped(const QString &t, int *pos) const;
  double round(double value) const;
  void stepBy(int steps);

public slots:
  void stepDown();
  void stepUp();

protected:
  QScopedPointer<qScienceSpinBoxPrivate> d_ptr;

  explicit qScienceSpinBox(qScienceSpinBoxPrivate *pimpl, QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qScienceSpinBox);
  Q_DISABLE_COPY(qScienceSpinBox);
};

#endif // qScienceSpinBox_h
