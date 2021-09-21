#ifndef __qSpinBoxDelegate_h
#define __qSpinBoxDelegate_h

// std includes
#include <limits>

// Colada includes
#include "qColadaAppExport.h"

// Qt includes
#include <QStyledItemDelegate>

class qSpinBoxDelegatePrivate;

class Q_COLADA_APP_EXPORT qSpinBoxDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qSpinBoxDelegate(int min = std::numeric_limits<int>::min(),
                            int max = std::numeric_limits<int>::max(),
                            int step = 1, QString prefix = QString(),
                            QString suffix = QString(),
                            QObject *parent = nullptr);

  virtual ~qSpinBoxDelegate() override;

  ///
  /// \brief setRange for int validator
  /// \param bottom
  /// \param top
  ///
  void setRange(int bottom, int top);

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
  QScopedPointer<qSpinBoxDelegatePrivate> d_ptr;

  explicit qSpinBoxDelegate(qSpinBoxDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qSpinBoxDelegate);
  Q_DISABLE_COPY(qSpinBoxDelegate);
};

#endif
