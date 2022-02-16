#ifndef __qSharedTableViews_h
#define __qSharedTableViews_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qSharedTableViewsPrivate;
class QTableView;

class Q_COLADA_APP_EXPORT qSharedTableViews : public QWidget {
  Q_OBJECT

public:
  explicit qSharedTableViews(QWidget *parent = nullptr);
  virtual ~qSharedTableViews() override;

public slots:
  /// \brief setOrientation Default orientation is horizontal
  /// \param orientation
  void setOrientation(Qt::Orientation orientation);
  void onAddToolBtnClicked();
  void onRemoveToolBtnClicked();
  void onFirstTableViewDoubleClicked(const QModelIndex &index);
  void onSecondTableViewDoubleClicked(const QModelIndex &index);

  /// Doesn't use proxy model
  QTableView* getFirstTableView();
  /// Doesn't use proxy model
  QTableView* getSecondTableView();

protected:
  QScopedPointer<qSharedTableViewsPrivate> d_ptr;

  explicit qSharedTableViews(qSharedTableViewsPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qSharedTableViews);
  Q_DISABLE_COPY(qSharedTableViews);
};

#endif
