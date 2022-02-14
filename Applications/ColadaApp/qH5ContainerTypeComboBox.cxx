// Colada includes
#include "qH5ContainerTypeComboBox.h"
#include "qH5ContainerTypeComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5core.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5ContainerTypeComboBoxPrivate::qH5ContainerTypeComboBoxPrivate(qH5ContainerTypeComboBox &q)
    : q_ptr(&q) {}

qH5ContainerTypeComboBoxPrivate::~qH5ContainerTypeComboBoxPrivate() {}

void qH5ContainerTypeComboBoxPrivate::init() {
  Q_Q(qH5ContainerTypeComboBox);
  auto flags = magic_enum::enum_names<h5geo::ContainerType>();
  for (const auto &name : flags) {
    q->addItem(QString::fromStdString(std::string(name)));
  }
}

qH5ContainerTypeComboBox::qH5ContainerTypeComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5ContainerTypeComboBoxPrivate(*this)) {
  Q_D(qH5ContainerTypeComboBox);
  d->init();
}

qH5ContainerTypeComboBox::qH5ContainerTypeComboBox(qH5ContainerTypeComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5ContainerTypeComboBox::~qH5ContainerTypeComboBox() {

}

bool qH5ContainerTypeComboBox::setH5ContainerType(unsigned containerType)
{
  auto containerType_opt =
      magic_enum::enum_cast<h5geo::ContainerType>(containerType);

  if (!containerType_opt.has_value())
    return false;

  for (int i = 0; i < this->count(); i++){
    auto opt =
        magic_enum::enum_cast<h5geo::ContainerType>(
          this->itemText(i).toStdString());
    if (!opt.has_value())
      continue;

    if (opt.value() == containerType_opt.value()){
      this->setCurrentIndex(i);
      return true;
    }
  }
  return false;
}

unsigned qH5ContainerTypeComboBox::getH5ContainerType()
{
  auto containerType_opt =
      magic_enum::enum_cast<h5geo::ContainerType>(this->currentText().toStdString());
  if (containerType_opt.has_value())
    return static_cast<unsigned>(containerType_opt.value());

  return 0;
}

