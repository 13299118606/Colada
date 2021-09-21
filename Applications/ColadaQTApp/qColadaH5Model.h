#ifndef __qColadaH5Model_h
#define __qColadaH5Model_h

#include <QAbstractItemModel>
#include <QVector>

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Item.h"

class qColadaH5ModelPrivate;

namespace h5gt {
class File;
}

class Q_COLADA_APP_EXPORT qColadaH5Model : public QAbstractItemModel {
  Q_OBJECT

public:
  explicit qColadaH5Model(QString title, QObject *parent = nullptr);
  virtual ~qColadaH5Model() override;

  virtual QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const override;
  virtual QModelIndex parent(const QModelIndex &child) const override;

  virtual int
  rowCount(const QModelIndex &parent = QModelIndex()) const override;
  virtual int
  columnCount(const QModelIndex &parent = QModelIndex()) const override;

  virtual QVariant data(const QModelIndex &index,
                        int role = Qt::DisplayRole) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value,
                       int role) override;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role) const override;

  virtual bool insertRows(int position, int rows,
                          const QModelIndex &parent = QModelIndex()) override;
  virtual bool removeRows(int position, int rows,
                          const QModelIndex &parent = QModelIndex()) override;

  virtual bool hasChildren(const QModelIndex &parent) const override;

  virtual bool canFetchMore(const QModelIndex &parent) const override;
  void fetchAllChildren(const QModelIndex &parent);

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex getIndex(qColadaH5Item *item) const;
  QList<qColadaH5Item *> getItemListToRoot(qColadaH5Item *item) const;
  QList<QModelIndex> getIndexListToRoot(qColadaH5Item *item) const;
  QList<QModelIndex>
  getIndexListToRoot(qColadaH5Item *item,
                     const QList<qColadaH5Item *> &itemListToRoot);
  void getFullChildList(qColadaH5Item *item,
                        QList<qColadaH5Item *> &fullChildList);
  void getFullChildIndexList(qColadaH5Item *item,
                             QList<QModelIndex> &fullChildIndexList);
  qColadaH5Item *itemFromIndex(const QModelIndex &index) const;
  qColadaH5Item *getRootItem() const;

  /// @brief Needs to be reimplemented by subclasses
  /// @param file 
  /// @return 
  virtual bool canAddH5File(h5gt::File file) const;

  bool addH5File(const QString &fullName);
  bool addH5File(h5gt::File file);
  void releaseCheckState(qColadaH5Item *topLevelItem);
  void updateCheckState(qColadaH5Item *topLevelItem);
  void sendAllChildDataChanged(
      qColadaH5Item *topLevelItem,
      const QVector<int> &roles = QVector<int>(Qt::CheckStateRole));
  void sendAllItemsToRootDataChanged(
      qColadaH5Item *lowLevelItem,
      const QVector<int> &roles = QVector<int>(Qt::CheckStateRole));
  void setCheckStateForItemStair(qColadaH5Item *item,
                                 Qt::CheckState checkState);

protected:
  QScopedPointer<qColadaH5ModelPrivate> d_ptr;

  explicit qColadaH5Model(
    qColadaH5ModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5Model);
  Q_DISABLE_COPY(qColadaH5Model);
};

#endif