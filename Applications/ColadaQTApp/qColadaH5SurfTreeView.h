#ifndef __qColadaH5SurfTreeView_h
#define __qColadaH5SurfTreeView_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5TreeView.h"

class qColadaH5SurfTreeViewPrivate;

class Q_COLADA_APP_EXPORT qColadaH5SurfTreeView : public qColadaH5TreeView {
  Q_OBJECT

public:
  explicit qColadaH5SurfTreeView(QWidget *parent = nullptr);
  virtual ~qColadaH5SurfTreeView() override;

protected:
  ///
  /// \brief for subclasses to add actions
  /// \param menu add actions here
  ///
  virtual void fillHdrMenu(QMenu *menu, QPoint pos) override;

private:
  virtual void hdrMenuRequested(QPoint pos) override;

public slots:
  virtual void onAddContainer() override;

protected:
  explicit qColadaH5SurfTreeView(qColadaH5SurfTreeViewPrivate *pimpl, QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5SurfTreeView);
  Q_DISABLE_COPY(qColadaH5SurfTreeView);
};

#endif
