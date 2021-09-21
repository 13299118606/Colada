#ifndef __qColadaH5WellTreeView_h
#define __qColadaH5WellTreeView_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5TreeView.h"

class qColadaH5WellTreeViewPrivate;

class Q_COLADA_APP_EXPORT qColadaH5WellTreeView : public qColadaH5TreeView {
  Q_OBJECT

public:
  explicit qColadaH5WellTreeView(QWidget *parent = nullptr);
  virtual ~qColadaH5WellTreeView() override;

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
  void onReadLas();
  void onReadDev();

protected:
  explicit qColadaH5WellTreeView(qColadaH5WellTreeViewPrivate *pimpl,
                                 QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5WellTreeView);
  Q_DISABLE_COPY(qColadaH5WellTreeView);
};

#endif
