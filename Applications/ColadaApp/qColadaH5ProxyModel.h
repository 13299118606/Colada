#ifndef __qColadaH5ProxyModel_h
#define __qColadaH5ProxyModel_h

#include <QSortFilterProxyModel>

// Colada includes
#include "qColadaAppExport.h"

class qColadaH5ProxyModelPrivate;

class Q_COLADA_APP_EXPORT qColadaH5ProxyModel : public QSortFilterProxyModel {
  Q_OBJECT

public:
  explicit qColadaH5ProxyModel(QObject *parent = nullptr);
  virtual ~qColadaH5ProxyModel() override;

public slots:
  void setShowCheckedItemsOnly(bool val);

  bool isShowCheckedOnly();

protected:
  virtual bool filterAcceptsRow(
      int source_row, const QModelIndex &source_parent) const override;

protected:
  QScopedPointer<qColadaH5ProxyModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaH5ProxyModel);
  Q_DISABLE_COPY(qColadaH5ProxyModel);
};

#endif
