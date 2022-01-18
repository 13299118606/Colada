// Colada includes
#include "qH5SeisDataTypeComboBox.h"
#include "qH5SeisDataTypeComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5core.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5SeisDataTypeComboBoxPrivate::qH5SeisDataTypeComboBoxPrivate(qH5SeisDataTypeComboBox &q)
    : q_ptr(&q) {}

qH5SeisDataTypeComboBoxPrivate::~qH5SeisDataTypeComboBoxPrivate() {}

void qH5SeisDataTypeComboBoxPrivate::init() {
  Q_Q(qH5SeisDataTypeComboBox);
  auto flags = magic_enum::enum_names<h5geo::SeisDataType>();
  for (const auto &name : flags) {
    q->addItem(QString::fromStdString(std::string(name)));
  }
}

qH5SeisDataTypeComboBox::qH5SeisDataTypeComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5SeisDataTypeComboBoxPrivate(*this)) {
  Q_D(qH5SeisDataTypeComboBox);
  d->init();
}

qH5SeisDataTypeComboBox::qH5SeisDataTypeComboBox(qH5SeisDataTypeComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5SeisDataTypeComboBox::~qH5SeisDataTypeComboBox() {

}

unsigned qH5SeisDataTypeComboBox::getH5SeisDataType()
{
  auto seisCreateType_opt =
      magic_enum::enum_cast<h5geo::SeisDataType>(this->currentText().toStdString());
  if (seisCreateType_opt.has_value())
    return static_cast<unsigned>(seisCreateType_opt.value());

  return 0;
}

