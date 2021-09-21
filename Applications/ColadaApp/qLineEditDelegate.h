#ifndef __qLineEditDelegate_h
#define __qLineEditDelegate_h

// Colada includes
#include "qColadaAppExport.h"

// Qt includes
#include <QStyledItemDelegate>

class qLineEditDelegatePrivate;
class QValidator;

class Q_COLADA_APP_EXPORT qLineEditDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qLineEditDelegate(QObject *parent = nullptr);
  explicit qLineEditDelegate(const QString &text, QObject *parent = nullptr);
  explicit qLineEditDelegate(const QString &text,
                             QValidator *validator,
                             QObject *parent = nullptr);

  virtual ~qLineEditDelegate() override;

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
  void setText(const QString &text);
  void setValidator(QValidator *validator);
  QString getText();
  QValidator* getValidator();

protected:
  QScopedPointer<qLineEditDelegatePrivate> d_ptr;

  explicit qLineEditDelegate(qLineEditDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qLineEditDelegate);
  Q_DISABLE_COPY(qLineEditDelegate);
};

#endif
