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
  Q_PROPERTY(int CRSCode READ CRSCode WRITE setCRSCode NOTIFY CRSCodeChanged)
public:
  explicit qCRSDropLineEdit(QWidget *parent = nullptr);
  virtual ~qCRSDropLineEdit() override;

  void setCRSName(QString name);
  void setCRSAuthName(QString authName);
  void setCRSCode(int code);

  QString CRSName();
  QString CRSAuthName();
  int CRSCode();

  QLineEdit* getCRSNameLineEdit();
  QLineEdit* getCRSAuthNameLineEdit();
  QLineEdit* getCRSCodeLineEdit();

signals:
  void CRSNameChanged(QString name);
  void CRSAuthNameChanged(QString authName);
  void CRSCodeChanged(int code);

public slots:

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
