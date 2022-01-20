#ifndef __qSharedListViews_h
#define __qSharedListViews_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qSharedListViewsPrivate;
class QListView;

class Q_COLADA_APP_EXPORT qSharedListViews : public QWidget {
  Q_OBJECT

public:
  explicit qSharedListViews(QWidget *parent = nullptr);
  virtual ~qSharedListViews() override;

public slots:
  /// \brief setOrientation Default orientation is horizontal
  /// \param orientation
  void setOrientation(Qt::Orientation orientation);
  void onAddToolBtnClicked();
  void onRemoveToolBtnClicked();
  void onFirstListViewDoubleClicked(const QModelIndex &index);
  void onSecondListViewDoubleClicked(const QModelIndex &index);

  /// Doesn't use proxy model
  QListView* getFirstListView();
  /// Doesn't use proxy model
  QListView* getSecondListView();

protected:
  QScopedPointer<qSharedListViewsPrivate> d_ptr;

  explicit qSharedListViews(qSharedListViewsPrivate *pimpl,
    QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qSharedListViews);
  Q_DISABLE_COPY(qSharedListViews);
};

#endif
