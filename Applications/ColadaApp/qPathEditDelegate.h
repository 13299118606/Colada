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
  explicit qPathEditDelegate(QObject *parent = nullptr);

  explicit qPathEditDelegate(
      const QString &label,
      ctkPathLineEdit::Filters filters = ctkPathLineEdit::AllEntries,
      QObject *parent = nullptr);

  explicit qPathEditDelegate(
      const QString &label, const QStringList &nameFilters,
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
  void setLabel(const QString &label);
  void setNameFilters(const QStringList &nameFilters);
  void setFilters(ctkPathLineEdit::Filters filters);
  QString getLabel();
  QStringList getNameFilters();
  ctkPathLineEdit::Filters getFilters();

protected:
  QScopedPointer<qPathEditDelegatePrivate> d_ptr;

  explicit qPathEditDelegate(qPathEditDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qPathEditDelegate);
  Q_DISABLE_COPY(qPathEditDelegate);
};

#endif
