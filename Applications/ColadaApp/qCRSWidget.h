#ifndef __qCRSWidget_h
#define __qCRSWidget_h

// Qt includes
#include <QWidget>

// Colada includes
#include "qColadaAppExport.h"

class qCRSWidgetPrivate;
class QItemSelection;
class ctkSearchBox;
class QLineEdit;
class QTableView;
class QTextBrowser;
class QSortFilterProxyModel;
class QStandardItemModel;
class QVBoxLayout;
class QSplitter;

class Q_COLADA_APP_EXPORT qCRSWidget : public QWidget
{
  Q_OBJECT

public:
  explicit qCRSWidget(QWidget* parent = nullptr);
  virtual ~qCRSWidget() override;

public slots:
  QString getUnits();
  ctkSearchBox* getSearchBox();
  QLineEdit* getUnitsLineEdit();
  QTableView* getTableView();
  QTextBrowser* getTextBrowser();
  QSortFilterProxyModel* getProxyModel();
  QStandardItemModel *getModel();
  QVBoxLayout *getMainLayout();
  QSplitter *getSplitter();

  void onSearchBoxEditingFinished();
  void onModelSelectionChanged(const QItemSelection &selected,
                               const QItemSelection &deselected);

protected:
  QScopedPointer<qCRSWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qCRSWidget);
  Q_DISABLE_COPY(qCRSWidget);
};

#endif
