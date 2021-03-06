#ifndef __qComboBoxDelegate_h
#define __qComboBoxDelegate_h

// Colada includes
#include "qColadaAppExport.h"

// Qt includes
#include <QStyledItemDelegate>

class qComboBoxDelegatePrivate;

class Q_COLADA_APP_EXPORT qComboBoxDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qComboBoxDelegate(QObject *parent = nullptr);
  explicit qComboBoxDelegate(const QStringList& texts,
                             QObject *parent = nullptr);
  virtual ~qComboBoxDelegate() override;

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
  void setTexts(QStringList texts);

protected:
  QScopedPointer<qComboBoxDelegatePrivate> d_ptr;

  explicit qComboBoxDelegate(qComboBoxDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qComboBoxDelegate);
  Q_DISABLE_COPY(qComboBoxDelegate);
};

#endif
