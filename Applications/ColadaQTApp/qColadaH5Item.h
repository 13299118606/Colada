#ifndef __qColadaH5Item_h
#define __qColadaH5Item_h

// std includes
#include <optional>

// Qt includes
#include <Qt>
#include <QScopedPointer>

// Colada includes
#include "qColadaAppExport.h"

class qColadaH5ItemPrivate;
class H5Base;
class H5BaseContainer;
class H5BaseObject;

class Q_COLADA_APP_EXPORT qColadaH5Item {
public:
  /// @brief Used in qColadaH5Model. To initialize `rootItem`
  /// pass `nullptr` to `itemData` and `parent`
  /// @param itemData if 
  /// @param parent 
  explicit qColadaH5Item(H5Base *itemData, qColadaH5Item *parent);
  virtual ~qColadaH5Item();

  int childCount() const;
  int childNumber() const;
  /// @brief rowCount is not the same as childCount.
  /// @return number of objects within Root/File/Group
  int rowCount() const;
  int columnCount() const;

  void appendChild(qColadaH5Item *item);
  void appendChildren(QVector<qColadaH5Item*> itemList);

  bool insertChild(qColadaH5Item *item, int position);
  bool removeChild(int position);
  bool removeChildren(int position, int count);

  ///
  /// \brief hasChild
  /// \param name is a visible name (itemData)
  /// \return
  ///
  bool hasChild(const QString &name) const;
  bool hasChildren() const;

  QString data() const;
  bool setData(const QString &newItemData);

  qint64 findRow(QVector<qColadaH5Item *> itemList, qColadaH5Item *item) const;

  ///
  /// \brief getChildByName returns nullptr if not exists
  /// \param name
  /// \return
  ///
  qColadaH5Item *getChildByName(const QString &name) const;
  QVector<qColadaH5Item *> getChildItems() const;
  qColadaH5Item *getChildItem(int number) const;
  qColadaH5Item *getParentItem() const;
  int getRow() const;
  QList<qColadaH5Item *> getItemListToRoot();

  H5Base *getItemData() const;
  H5BaseContainer *getGeoContainer() const;
  H5BaseObject *getGeoObject() const;


  bool setChildItems(QVector<qColadaH5Item *> childItems);
  bool setParentItem(qColadaH5Item *parentItem);
  // I NEED TO IMPLEMENT THIS CORRECTLY
  void setCheckState(Qt::CheckState checkState);
  /// @brief Used in models in canFetchMore() function
  /// @param val 
  /// @return 
  void setMapped(bool val);

  bool isRoot() const;
  bool isGeoContainer() const;
  bool isGeoObject() const;

  // I NEED TO IMPLEMENT THIS CORRECTLY
  Qt::CheckState checkState() const;
  bool isEvenOneChildChecked();
  /// @brief Trigger for canFetchMore() of a model
  /// @return 
  bool isMapped() const;

  static QString fullName2ItemData(const QString &fullName);

  bool operator==(const qColadaH5Item &another) const;

protected:
  QScopedPointer<qColadaH5ItemPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaH5Item);
  Q_DISABLE_COPY(qColadaH5Item);

  friend class qColadaH5Model;
};

#endif