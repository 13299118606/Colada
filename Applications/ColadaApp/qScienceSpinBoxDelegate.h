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
  explicit qScienceSpinBoxDelegate(QObject *parent = nullptr);

  explicit qScienceSpinBoxDelegate(double min, double max, double step,
                                   const QString &prefix = QString(),
                                   const QString &suffix = QString(),
                                   QObject *parent = nullptr);

  virtual ~qScienceSpinBoxDelegate() override;

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
  ///
  /// \brief setRange for double validator
  /// \param bottom
  /// \param top
  ///
  void setRange(double bottom, double top);
  void setMinValue(double val);
  void setMaxValue(double val);
  void setStep(double val);
  void setPrefix(const QString &prefix);
  void setSuffix(const QString &suffix);
  double getMinValue();
  double getMaxValue();
  double getStep();
  QString getPrefix();
  QString getSuffix();

protected:
  QScopedPointer<qScienceSpinBoxDelegatePrivate> d_ptr;

  explicit qScienceSpinBoxDelegate(qScienceSpinBoxDelegatePrivate *pimpl, QObject *parent);

private:
  Q_DECLARE_PRIVATE(qScienceSpinBoxDelegate);
  Q_DISABLE_COPY(qScienceSpinBoxDelegate);
};

#endif
