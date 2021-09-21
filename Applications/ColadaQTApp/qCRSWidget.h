#ifndef __qCRSWidget_h
#define __qCRSWidget_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qCRSWidgetPrivate;
class QItemSelection;
class ctkSearchBox;
class QTableView;
class QTextBrowser;
class QSortFilterProxyModel;
class QStandardItemModel;

class Q_COLADA_APP_EXPORT qCRSWidget : public QWidget
{
  Q_OBJECT

public:
  explicit qCRSWidget(QWidget* parent = nullptr);
  virtual ~qCRSWidget() override;

  ctkSearchBox* getSearchBox();
  QTableView* getTableView();
  QTextBrowser* getTextBrowser();
  QSortFilterProxyModel* getProxyModel();
  QStandardItemModel* getModel();

public slots:
  void onSearchBoxEditingFinished();
  void onModelSelectionChanged(
    const QItemSelection& selected,
    const QItemSelection& deselected);

protected:
  QScopedPointer<qCRSWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qCRSWidget);
  Q_DISABLE_COPY(qCRSWidget);
};

#endif
