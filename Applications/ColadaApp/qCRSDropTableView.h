#ifndef __qCRSDropTableView_h
#define __qCRSDropTableView_h

// Colada includes
#include "qCopyPasteTableView.h"

class qCRSDropTableViewPrivate;

class Q_COLADA_APP_EXPORT qCRSDropTableView : public qCopyPasteTableView {
  Q_OBJECT

public:
  explicit qCRSDropTableView(QWidget *parent = nullptr);
  virtual ~qCRSDropTableView() override;

protected:
  virtual void dropEvent(QDropEvent *event) override;

protected:
  explicit qCRSDropTableView(qCRSDropTableViewPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qCRSDropTableView);
  Q_DISABLE_COPY(qCRSDropTableView);
};

#endif // qCRSDropTableView_h
