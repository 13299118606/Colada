#ifndef __qCopyPasteTableView_h
#define __qCopyPasteTableView_h

// Qt includes
#include <QTableView>

// Colada includes
#include "qColadaAppExport.h"

class qCopyPasteTableViewPrivate;

class Q_COLADA_APP_EXPORT qCopyPasteTableView : public QTableView {
  Q_OBJECT

public:
  explicit qCopyPasteTableView(QWidget *parent = nullptr);
  virtual ~qCopyPasteTableView() override;

  ///
  /// \brief updateCellData updates displayed data according to Delegates and
  /// Validators \param index
  ///
  void updateCellData(const QModelIndex &index);

protected:
  virtual void keyPressEvent(QKeyEvent *event) override;

public slots:
  void doCut();
  void doCopy();
  void doPaste();
  void doDelete();

protected:
  QScopedPointer<qCopyPasteTableViewPrivate> d_ptr;

  explicit qCopyPasteTableView(qCopyPasteTableViewPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qCopyPasteTableView);
  Q_DISABLE_COPY(qCopyPasteTableView);
};

#endif // qCopyPasteTableView_h
