#ifndef __qColadaH5TreeView_h
#define __qColadaH5TreeView_h

// Qt includes
#include <QTreeView>

// Colada includes
#include "qColadaAppExport.h"

class qColadaH5TreeViewPrivate;
class QMenu;
class QPoint;

class Q_COLADA_APP_EXPORT qColadaH5TreeView : public QTreeView {
  Q_OBJECT

public:
  explicit qColadaH5TreeView(QWidget *parent = nullptr);
  virtual ~qColadaH5TreeView() override;

  bool addContainer(const QString &fileName);
  bool removeContainer(const QString &fileName);
  bool refreshContainer(const QString &fileName);

protected:
  ///
  /// \brief for subclasses to add actions
  /// \param menu add actions here
  virtual void fillHdrMenu(QMenu *menu, const QPoint &pos);
  virtual void fillTreeViewMenu(QMenu *menu, const QPoint &pos);

private:
  virtual void hdrMenuRequested(const QPoint &pos);
  virtual void treeViewMenuRequested(const QPoint &pos);

public slots:
  virtual void onAddContainer();
  virtual void onRemoveContainer();
  virtual void onRefreshContainer();
  virtual void onExpandSelected();
  virtual void onCollapseSelected();

protected:
  QScopedPointer<qColadaH5TreeViewPrivate> d_ptr;

  explicit qColadaH5TreeView(qColadaH5TreeViewPrivate *pimpl,
                             QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qColadaH5TreeView);
  Q_DISABLE_COPY(qColadaH5TreeView);
};

#endif
