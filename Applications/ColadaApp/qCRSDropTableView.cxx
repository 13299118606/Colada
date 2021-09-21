// Colada includes
#include "qCRSDropTableView.h"
#include "qCRSDropTableView_p.h"

// Qt includes
#include <QDropEvent>
#include <QHeaderView>
#include <QMimeData>
#include <QProxyStyle>

/* this STYLE class is nested. It is used to show DROP INDICATOR as a ROW
 * RECTANGLE */
class qCRSDropTableViewStyle : public QProxyStyle {
public:
  qCRSDropTableViewStyle(QStyle *style = nullptr);

  void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                     QPainter *painter, const QWidget *widget = nullptr) const;
};

qCRSDropTableViewStyle::qCRSDropTableViewStyle(QStyle *style) : QProxyStyle(style) {}

void qCRSDropTableViewStyle::drawPrimitive(PrimitiveElement element,
                                       const QStyleOption *option,
                                       QPainter *painter,
                                       const QWidget *widget) const {
  if (element == QStyle::PE_IndicatorItemViewItemDrop &&
      !option->rect.isNull()) {
    QStyleOption opt(*option);
    opt.rect.setLeft(0);
    QTableView *tableView =
        qobject_cast<QTableView *>(const_cast<QWidget *>(widget));
    if (tableView && tableView->model()) {
      int colNum = tableView->model()->columnCount();
      opt.rect.setRight(tableView->columnViewportPosition(colNum - 1) +
                        tableView->columnWidth(colNum - 1));
    } else if (widget) {
      opt.rect.setRight(widget->width());
    }
    QProxyStyle::drawPrimitive(element, &opt, painter, widget);
    return;
  }
  QProxyStyle::drawPrimitive(element, option, painter, widget);
}

qCRSDropTableViewPrivate::qCRSDropTableViewPrivate(
  qCRSDropTableView &q)
    : Superclass(q) {}

qCRSDropTableViewPrivate::~qCRSDropTableViewPrivate() {}

void qCRSDropTableViewPrivate::init() { 
  Q_Q(qCRSDropTableView);
  this->Superclass::init();
  q->setStyle(new qCRSDropTableViewStyle(q->style()));
}

qCRSDropTableView::qCRSDropTableView(QWidget *parent) 
  : qCopyPasteTableView(new qCRSDropTableViewPrivate(*this), parent) {
  Q_D(qCRSDropTableView);
  d->init();
}

qCRSDropTableView::qCRSDropTableView(qCRSDropTableViewPrivate *pimpl,
  QWidget *parent)
    : qCopyPasteTableView(pimpl, parent) {
  // init() is called by derived class.
}

qCRSDropTableView::~qCRSDropTableView() {}

void qCRSDropTableView::dropEvent(QDropEvent *event) {
  const QMimeData *mime = event->mimeData();
  QStringList typeList = model()->mimeTypes();

  if (typeList.isEmpty())
    return;

  int sourceRow, sourceCol;
  QByteArray encodedData = mime->data(typeList[0]);
  QDataStream stream(&encodedData, QIODevice::ReadOnly);
  stream >> sourceRow >> sourceCol;

  QTableView *sourceTableView = qobject_cast<QTableView *>(event->source());

  if (sourceTableView == nullptr)
    return;

  QString authName = sourceTableView->model()
                         ->data(sourceTableView->model()->index(sourceRow, 1))
                         .toString();
  QString code = sourceTableView->model()
                     ->data(sourceTableView->model()->index(sourceRow, 2))
                     .toString();

  int row = indexAt(event->pos()).row();
  for (int col = 0; col < model()->columnCount(); col++) {
    QString colName = model()->headerData(col, Qt::Horizontal).toString();
    if (colName.compare("CRS", Qt::CaseInsensitive) == 0) {
      QModelIndex index = model()->index(row, col);
      model()->setData(index, authName + ":" + code);
      break;
    }
  }
}
