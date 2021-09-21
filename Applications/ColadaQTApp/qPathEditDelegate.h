#ifndef __qPathEditDelegate_h
#define __qPathEditDelegate_h

// Colada includes
#include "qColadaAppExport.h"

// CTK includes
#include <ctkPathLineEdit.h>

// Qt includes
#include <QStyledItemDelegate>

class qPathEditDelegatePrivate;

class Q_COLADA_APP_EXPORT qPathEditDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qPathEditDelegate(
      QString label = QString(), QStringList nameFilters = QStringList(),
      ctkPathLineEdit::Filters filters = ctkPathLineEdit::AllEntries,
      QObject *parent = nullptr);

  virtual ~qPathEditDelegate() override;

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
  QScopedPointer<qPathEditDelegatePrivate> d_ptr;

  explicit qPathEditDelegate(qPathEditDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qPathEditDelegate);
  Q_DISABLE_COPY(qPathEditDelegate);
};

#endif
