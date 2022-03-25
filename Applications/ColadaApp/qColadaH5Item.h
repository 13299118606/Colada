#ifndef __qColadaH5Item_h
#define __qColadaH5Item_h

// std includes
#include <optional>

// Qt includes
#include <Qt>
#include <QScopedPointer>

// Colada includes
#include "qColadaAppExport.h"

// h5gt includes
#include <h5gt/H5File.hpp>
#include <h5gt/H5Group.hpp>

class qColadaH5ItemPrivate;
class H5Base;
class H5BaseContainer;
class H5BaseObject;

class Q_COLADA_APP_EXPORT qColadaH5Item {
public:
  /// \brief Used in qColadaH5Model. To initialize `rootItem`
  /// pass `nullptr` as `parent`.
  /// To initialize file pass `rootItem` as `parent`.
  /// \param itemData
  /// \param parent
  explicit qColadaH5Item(const QString& itemData, qColadaH5Item *parent);
  virtual ~qColadaH5Item();

  int childCount() const;
  int childNumber() const;
  /// \brief rowCount is not the same as childCount.
  /// \return number of objects within Root/File/Group
  int rowCount() const;
  int columnCount() const;

  void appendChild(qColadaH5Item *item);
  void appendChildren(QVector<qColadaH5Item*> itemList);

  /// \brief insertChild if position is out of bounds then prepend/append
  /// \param item
  /// \param position
  void insertChild(qColadaH5Item *item, int position);
  void removeChild(int position, bool destroy = false);
  void removeChildren(int position, int count, bool destroy = false);

  /// return bool in case the item is not found
  bool removeChild(qColadaH5Item *item, bool destroy = false);
  bool removeChildByName(const QString& name, bool destroy = false);

  /// \brief hasChild
  /// \param name is a visible name (itemData)
  /// \return
  bool hasChild(const QString &name) const;
  bool hasChildren() const;

  QString data() const;
  QString rawData() const;
  bool setData(const QString &newItemData);

  int getChildRow(qColadaH5Item *item);
  int getChildRow(const QString& name);

  /// \brief getPath retrieve the path within container
  /// \return
  QString getPath() const;
  qColadaH5Item *getContainerItem() const;
  std::optional<h5gt::File> getH5File() const;
  std::optional<h5gt::Group> getObjG() const;

  /// \brief returns nullptr if not exists
  /// \param name
  /// \return
  qColadaH5Item *getChildByName(const QString &name) const;
  QVector<qColadaH5Item *> getChildren() const;
  qColadaH5Item *getChild(int number) const;
  qColadaH5Item *getParent() const;
  int getRow() const;
  int getColumn() const;
  QList<qColadaH5Item *> getItemListToRoot();

  bool setChildren(QVector<qColadaH5Item *> childItems);
  bool setParent(qColadaH5Item *parentItem);
  void setCheckState(Qt::CheckState checkState);
  /// \brief used in models in canFetchMore() function
  /// \param val
  /// \return
  void setMapped(bool val);

  bool isRoot() const;
  bool isContainer() const;
  bool isObject() const;
  /// true if container or object exist within hdf5 file
  bool exist() const;

  void setObjectType(unsigned type);
  void setChildCountInGroup(size_t n);
  void setFlags(Qt::ItemFlags flags);

  unsigned getObjectType() const;
  size_t getChildCountInGroup() const;
  Qt::ItemFlags getFlags() const;

  // I NEED TO IMPLEMENT THIS CORRECTLY
  Qt::CheckState checkState() const;
  bool isEvenOneChildChecked();
  /// \brief Trigger for canFetchMore() of a model
  /// \return
  bool isMapped() const;

  /// h5gt LinkType
  void setLinkType(h5gt::LinkType linkType);
  h5gt::LinkType getLinkType() const;
  bool isLinkTypeHard() const;
  bool isLinkTypeSoft() const;
  bool isLinkTypeExternal() const;

  // only write to stream is possible. We cant read from stream and set the data to item
  void write(QDataStream &out) const;

  bool isSame(qColadaH5Item* another);
  bool operator==(const qColadaH5Item &another) const;
  bool operator!=(const qColadaH5Item &another) const;

protected:
  QScopedPointer<qColadaH5ItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaH5Item);
  Q_DISABLE_COPY(qColadaH5Item);

  friend class qColadaH5Model;
};

Q_COLADA_APP_EXPORT QDataStream &operator<<(QDataStream &out, const qColadaH5Item &item);

#endif
