#ifndef __qCRSDropLineEdit_h
#define __qCRSDropLineEdit_h

// Qt includes
#include <QWidget>

#include "qColadaAppExport.h"

class qCRSDropLineEditPrivate;
class QLineEdit;

class Q_COLADA_APP_EXPORT qCRSDropLineEdit : public QWidget {
  Q_OBJECT

  /// Holds the current CRS name for the settings.
  Q_PROPERTY(QString CRSName READ CRSName WRITE setCRSName NOTIFY CRSNameChanged)
  /// Holds the current CRS auth name for the settings.
  Q_PROPERTY(QString CRSAuthName READ CRSAuthName WRITE setCRSAuthName NOTIFY CRSAuthNameChanged)
  /// Holds the current CRS code for the settings.
  Q_PROPERTY(QString CRSCode READ CRSCode WRITE setCRSCode NOTIFY CRSCodeChanged)
public:
  explicit qCRSDropLineEdit(QWidget *parent = nullptr);
  virtual ~qCRSDropLineEdit() override;

  void setCRS(const QString& name, const QString& authName, const QString& code);
  void setCRSName(const QString& name);
  void setCRSAuthName(const QString& authName);
  void setCRSCode(const QString& code);

  QString CRSName();
  QString CRSAuthName();
  QString CRSCode();

  QLineEdit* getCRSNameLineEdit();
  QLineEdit* getCRSAuthNameLineEdit();
  QLineEdit* getCRSCodeLineEdit();

signals:
  void CRSChanged(const QString& name, const QString& authName, const QString& code);
  void CRSNameChanged(const QString& name);
  void CRSAuthNameChanged(const QString& authName);
  void CRSCodeChanged(const QString& code);

public slots:

protected slots:
  void onNameLineEditChanged(const QString& name);
  void onAuthNameLineEditChanged(const QString& authName);
  void onCodeLineEditChanged(const QString& code);

protected:
  QScopedPointer<qCRSDropLineEditPrivate> d_ptr;

  explicit qCRSDropLineEdit(qCRSDropLineEditPrivate *pimpl,
    QWidget *parent);

  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dropEvent(QDropEvent *event) override;

private:
  Q_DECLARE_PRIVATE(qCRSDropLineEdit);
  Q_DISABLE_COPY(qCRSDropLineEdit);
};

#endif // qCRSDropLineEdit_h
