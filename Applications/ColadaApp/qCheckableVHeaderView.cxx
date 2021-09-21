// Colada includes
#include "qCheckableVHeaderView.h"
#include "qCheckableVHeaderView_p.h"

qCheckableVHeaderViewPrivate::qCheckableVHeaderViewPrivate(
    qCheckableVHeaderView &q)
    : q_ptr(&q) {}

qCheckableVHeaderViewPrivate::~qCheckableVHeaderViewPrivate() {}

void qCheckableVHeaderViewPrivate::init() { Q_Q(qCheckableVHeaderView); }

qCheckableVHeaderView::qCheckableVHeaderView(QWidget *parent)
    : ctkCheckableHeaderView(Qt::Vertical, parent),
      d_ptr(new qCheckableVHeaderViewPrivate(*this)) {
  Q_D(qCheckableVHeaderView);
  d->init();
}

qCheckableVHeaderView::qCheckableVHeaderView(qCheckableVHeaderViewPrivate *pimpl,
    QWidget *parent)
    : ctkCheckableHeaderView(Qt::Vertical, parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qCheckableVHeaderView::~qCheckableVHeaderView() {}

Qt::CheckState qCheckableVHeaderView::getCheckState(int section) const {
  return this->checkState(section);
}