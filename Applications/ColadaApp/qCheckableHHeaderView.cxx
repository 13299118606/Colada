// Colada includes
#include "qCheckableHHeaderView.h"
#include "qCheckableHHeaderView_p.h"

qCheckableHHeaderViewPrivate::qCheckableHHeaderViewPrivate(
    qCheckableHHeaderView &q)
    : q_ptr(&q) {}

qCheckableHHeaderViewPrivate::~qCheckableHHeaderViewPrivate() {}

void qCheckableHHeaderViewPrivate::init() { Q_Q(qCheckableHHeaderView); }

qCheckableHHeaderView::qCheckableHHeaderView(QWidget *parent)
    : ctkCheckableHeaderView(Qt::Horizontal, parent),
      d_ptr(new qCheckableHHeaderViewPrivate(*this)) {
  Q_D(qCheckableHHeaderView);
  d->init();
}

qCheckableHHeaderView::qCheckableHHeaderView(qCheckableHHeaderViewPrivate *pimpl,
    QWidget *parent)
    : ctkCheckableHeaderView(Qt::Horizontal, parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qCheckableHHeaderView::~qCheckableHHeaderView() {}

Qt::CheckState qCheckableHHeaderView::getCheckState(int section) const {
  return this->checkState(section);
}