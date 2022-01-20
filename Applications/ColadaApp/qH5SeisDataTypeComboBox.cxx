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

bool qH5SeisDataTypeComboBox::setH5SeisDataType(unsigned dtype){
  auto seisDataType_opt =
      magic_enum::enum_cast<h5geo::SeisDataType>(dtype);

  if (!seisDataType_opt.has_value())
    return false;

  for (int i = 0; i < this->count(); i++){
    auto opt =
        magic_enum::enum_cast<h5geo::SeisDataType>(
          this->itemText(i).toStdString());
    if (!opt.has_value())
      continue;

    if (opt.value() == seisDataType_opt.value()){
      this->setCurrentIndex(i);
      return true;
    }
  }
  return false;
}

unsigned qH5SeisDataTypeComboBox::getH5SeisDataType()
{
  auto seisDataType_opt =
      magic_enum::enum_cast<h5geo::SeisDataType>(this->currentText().toStdString());
  if (seisDataType_opt.has_value())
    return static_cast<unsigned>(seisDataType_opt.value());

  return 0;
}

