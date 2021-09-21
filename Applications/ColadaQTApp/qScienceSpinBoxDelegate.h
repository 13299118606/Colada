#ifndef __qScienceSpinBoxDelegate_h
#define __qScienceSpinBoxDelegate_h

// std includes
#include <limits>

// Colada includes
#include "qColadaAppExport.h"

// Qt includes
#include <QStyledItemDelegate>

class qScienceSpinBoxDelegatePrivate;

class Q_COLADA_APP_EXPORT qScienceSpinBoxDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  explicit qScienceSpinBoxDelegate(int min = std::numeric_limits<int>::min(),
                            int max = std::numeric_limits<int>::max(),
                            int step = 1, QString prefix = QString(),
                            QString suffix = QString(),
                            QObject *parent = nullptr);

  virtual ~qScienceSpinBoxDelegate() override;

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
  QScopedPointer<qScienceSpinBoxDelegatePrivate> d_ptr;

  explicit qScienceSpinBoxDelegate(qScienceSpinBoxDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qScienceSpinBoxDelegate);
  Q_DISABLE_COPY(qScienceSpinBoxDelegate);
};

#endif
