#ifndef __qCheckableHHeaderView_h
#define __qCheckableHHeaderView_h

// ctk includes
#include <ctkCheckableHeaderView.h>

// Colada includes
#include "qColadaAppExport.h"

class qCheckableHHeaderViewPrivate;

class Q_COLADA_APP_EXPORT qCheckableHHeaderView : public ctkCheckableHeaderView {
  Q_OBJECT

public:
  /// @brief creates horizontal header view
  explicit qCheckableHHeaderView(QWidget *parent = nullptr);
  virtual ~qCheckableHHeaderView() override;

public slots:
  Qt::CheckState getCheckState(int section) const;

protected:
  QScopedPointer<qCheckableHHeaderViewPrivate> d_ptr;

  explicit qCheckableHHeaderView(qCheckableHHeaderViewPrivate *pimpl,
                                 QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qCheckableHHeaderView);
  Q_DISABLE_COPY(qCheckableHHeaderView);
};

#endif