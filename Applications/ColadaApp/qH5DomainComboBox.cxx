// Colada includes
#include "qH5DomainComboBox.h"
#include "qH5DomainComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5core.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5DomainComboBoxPrivate::qH5DomainComboBoxPrivate(qH5DomainComboBox &q)
    : q_ptr(&q) {}

qH5DomainComboBoxPrivate::~qH5DomainComboBoxPrivate() {}

void qH5DomainComboBoxPrivate::init() {
  Q_Q(qH5DomainComboBox);
  auto flags = magic_enum::enum_names<h5geo::Domain>();
  for (const auto &name : flags) {
    q->addItem(QString::fromStdString(std::string(name)));
  }
}

qH5DomainComboBox::qH5DomainComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5DomainComboBoxPrivate(*this)) {
  Q_D(qH5DomainComboBox);
  d->init();
}

qH5DomainComboBox::qH5DomainComboBox(qH5DomainComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5DomainComboBox::~qH5DomainComboBox() {

}

unsigned qH5DomainComboBox::getH5Domain()
{
  auto seisCreateType_opt =
      magic_enum::enum_cast<h5geo::Domain>(this->currentText().toStdString());
  if (seisCreateType_opt.has_value())
    return static_cast<unsigned>(seisCreateType_opt.value());

  return 0;
}

