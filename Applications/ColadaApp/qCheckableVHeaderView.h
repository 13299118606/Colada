#ifndef __qCheckableVHeaderView_h
#define __qCheckableVHeaderView_h

// ctk includes
#include <ctkCheckableHeaderView.h>

// Colada includes
#include "qColadaAppExport.h"

class qCheckableVHeaderViewPrivate;

class Q_COLADA_APP_EXPORT qCheckableVHeaderView : public ctkCheckableHeaderView {
  Q_OBJECT

public:
  /// @brief default constructor creates horizontal orientation (needs for
  /// PythonQt)
  explicit qCheckableVHeaderView(QWidget *parent = nullptr);
  virtual ~qCheckableVHeaderView() override;

public slots:
  Qt::CheckState getCheckState(int section) const;

protected:
  QScopedPointer<qCheckableVHeaderViewPrivate> d_ptr;

  explicit qCheckableVHeaderView(qCheckableVHeaderViewPrivate *pimpl,
                                 QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qCheckableVHeaderView);
  Q_DISABLE_COPY(qCheckableVHeaderView);
};

#endif