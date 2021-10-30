#ifndef __qColadaH5MapTreeView_h
#define __qColadaH5MapTreeView_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5TreeView.h"

class qColadaH5MapTreeViewPrivate;

class Q_COLADA_APP_EXPORT qColadaH5MapTreeView : public qColadaH5TreeView {
  Q_OBJECT

public:
  explicit qColadaH5MapTreeView(QWidget *parent = nullptr);
  virtual ~qColadaH5MapTreeView() override;

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
  void onReadMap();

protected:
  explicit qColadaH5MapTreeView(qColadaH5MapTreeViewPrivate *pimpl, QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5MapTreeView);
  Q_DISABLE_COPY(qColadaH5MapTreeView);
};

#endif
