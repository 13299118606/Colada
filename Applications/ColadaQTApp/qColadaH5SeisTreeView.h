#ifndef __qColadaH5SeisTreeView_h
#define __qColadaH5SeisTreeView_h

// Colada includes
#include "qColadaAppExport.h"
#include "qColadaH5TreeView.h"

class qColadaH5SeisTreeViewPrivate;

class Q_COLADA_APP_EXPORT qColadaH5SeisTreeView : public qColadaH5TreeView {
  Q_OBJECT

public:
  explicit qColadaH5SeisTreeView(QWidget *parent = nullptr);
  virtual ~qColadaH5SeisTreeView() override;

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
  void onReadSegy();

protected:
  explicit qColadaH5SeisTreeView(qColadaH5SeisTreeViewPrivate *pimpl,
                                 QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5SeisTreeView);
  Q_DISABLE_COPY(qColadaH5SeisTreeView);
};

#endif
