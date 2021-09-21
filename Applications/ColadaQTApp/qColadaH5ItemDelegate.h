#ifndef __qColadaH5ItemDelegate_h
#define __qColadaH5ItemDelegate_h

#include <QStyledItemDelegate>

// Colada includes
#include "qColadaAppExport.h"

class qColadaH5ItemDelegatePrivate;

class Q_COLADA_APP_EXPORT qColadaH5ItemDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qColadaH5ItemDelegate(QObject *parent = nullptr);
  virtual ~qColadaH5ItemDelegate() override;

protected:
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

signals:

public slots:

protected:
  QScopedPointer<qColadaH5ItemDelegatePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaH5ItemDelegate);
  Q_DISABLE_COPY(qColadaH5ItemDelegate);
};

#endif
