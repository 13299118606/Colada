#ifndef __qColadaH5Model_h
#define __qColadaH5Model_h

// Qt includes
#include <QAbstractItemModel>
#include <QVector>

// h5gt includes
#include <h5gt/H5File.hpp> // File must be included or moc-file gives error

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5Item.h"

// CTK includes
#include <ctkVTKObject.h>

class qColadaH5ModelPrivate;
class vtkObject;
class vtkMRMLNode;

namespace Colada {


enum ColadaRoles{
  CheckBoxClickedRole=Qt::UserRole,
//  myRole2,
//  myRole3
};


} // Colada namespace

class Q_COLADA_APP_EXPORT qColadaH5Model : public QAbstractItemModel {
  Q_OBJECT
  QVTK_OBJECT

public:
  explicit qColadaH5Model(QObject *parent = nullptr);
  explicit qColadaH5Model(const QString& title, QObject *parent = nullptr);
  virtual ~qColadaH5Model() override;

public slots:
  void setTitle(const QString &title);
  QString getTitle();

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

  bool insertRows(int position, int rows,
                  qColadaH5Item *parentItem);

  virtual bool removeRows(int position, int rows,
                          const QModelIndex &parent = QModelIndex()) override;

  bool removeRows(int position, int rows,
                  qColadaH5Item *parentItem);

  bool moveItem(qColadaH5Item *parentItem,
                qColadaH5Item *item,
                int position, bool moveH5Link = true);

  /// Check if the item can be moved to the new parent `parentItem`.
  /// If `moveH5Link` then also check if it is possible to rename hdf5 link
  bool canBeMoved(qColadaH5Item*& parentItem, qColadaH5Item*& item, bool moveH5Link = true);

  virtual bool hasChildren(const QModelIndex &parent) const override;

  virtual bool canFetchMore(const QModelIndex &parent) const override;

  void refetch(const QModelIndex &parent);
  void fetchAllChildren(const QModelIndex &parent);

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex getIndex(qColadaH5Item *item) const;
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

  /// brief Needs to be reimplemented by subclasses to prevent from adding
  /// container of different geo type. No need to check if the item already added,
  /// that is done internally
  virtual bool canAddH5File(const h5gt::File& file) const;

  qColadaH5Item* findItem(const QString &fileName);
  qColadaH5Item* findItem(const h5gt::File& file);
  qColadaH5Item* findItem(const QString &fileName, const QString& objName, bool fetch = true);
  qColadaH5Item* findItem(const h5gt::Group& objG, bool fetch = true);
  qColadaH5Item* findItem(vtkMRMLNode* node, bool fetch = true);

  /// if 'row < 0' prepend; if 'row > childCount()' append
  qColadaH5Item* insertH5File(const QString &fileName, int row);
  qColadaH5Item* insertH5File(const h5gt::File& file, int row);
  qColadaH5Item* addH5File(const QString &fileName);
  qColadaH5Item* addH5File(const h5gt::File& file);
  bool removeH5File(const QString &fileName);
  bool removeH5File(const h5gt::File& file);

  /// if 'row < 0' prepend; if 'row > childCount()' append
  qColadaH5Item* insertH5Object(const QString& fileName, const QString& objName, int row);
  qColadaH5Item* insertH5Object(const h5gt::Group& objG, int row);
  qColadaH5Item* addH5Object(const QString& fileName, const QString& objName);
  qColadaH5Item* addH5Object(const h5gt::Group& objG);
  bool removeH5Object(const QString& fileName, const QString& objName, bool unlink = false);
  bool removeH5Object(const h5gt::Group& objG, bool unlink = false);

  /// Called every time model creates new items. Aimed to set props in subclasses
  virtual void initItemToBeInserted(qColadaH5Item* item) const;

  bool removeItem(qColadaH5Item* item, bool unlink = false);

  void releaseCheckState(qColadaH5Item *topLevelItem);
  void updateCheckState(qColadaH5Item *topLevelItem);
  void sendItemDataChanged(
      qColadaH5Item* item,
      const QVector<int> &roles = QVector<int>(Qt::CheckStateRole));
  void sendAllChildDataChanged(
      qColadaH5Item *topLevelItem,
      const QVector<int> &roles = QVector<int>(Qt::CheckStateRole));
  void sendAllItemsToRootDataChanged(
      qColadaH5Item *lowLevelItem,
      const QVector<int> &roles = QVector<int>(Qt::CheckStateRole));
  void setCheckStateForItemStair(qColadaH5Item *item,
                                 Qt::CheckState checkState);

  /// When fetching items at the time when some nodes already added
  /// we need to init checkstate for the items.
  /// Also if an item is renamed we need to update checkstate.
  void updateItemsCheckState(const QVector<qColadaH5Item*>& items);

  QStringList mimeTypes() const override;
  QMimeData *mimeData(const QModelIndexList &indexes) const override;

  /// Must be virtual to be able to reimplement for custom nodes
  virtual std::optional<h5gt::Group> h5GroupFromNode(vtkMRMLNode* node);

  virtual void onMRMLSceneNodeAdded(vtkObject*, vtkObject* node);
  virtual void onMRMLSceneNodeRemoved(vtkObject*, vtkObject* node);

  /// to connect with qColadaAppMainWindow::emitH5FileToBeAdded
  virtual void onH5FileToBeAdded(const QString& fileName);
  /// to connect with qColadaAppMainWindow::emitH5FileToBeRemoved
  virtual void onH5FileToBeRemoved(const QString& fileName);
  /// to connect with qColadaAppMainWindow::emitH5ObjectToBeAdded
  virtual void onH5ObjectToBeAdded(const QString& fileName, const QString& objName);
  /// to connect with qColadaAppMainWindow::emitH5ObjectToBeRemoved
  virtual void onH5ObjectToBeRemoved(const QString& fileName, const QString& objName);

  virtual bool canDropMimeData(
      const QMimeData *data,
      Qt::DropAction action,
      int row,
      int column,
      const QModelIndex &parent) const override;

  virtual bool dropMimeData(
      const QMimeData *data,
      Qt::DropAction action,
      int row,
      int column,
      const QModelIndex &parent) override;

  /// Fill all args and return 'true' if item is able to be moved
  bool canDropMimeDataAndPrepare(
      const QMimeData *data,
      int row,
      int column,
      const QModelIndex &parent,
      qColadaH5Item*& parentItem,
      qColadaH5Item*& item);

protected:
  QScopedPointer<qColadaH5ModelPrivate> d_ptr;

  explicit qColadaH5Model(
    qColadaH5ModelPrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5Model);
  Q_DISABLE_COPY(qColadaH5Model);
};

#endif
