// Colada includes
#include "qH5ObjectTypeComboBox.h"
#include "qH5ObjectTypeComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5util.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5ObjectTypeComboBoxPrivate::qH5ObjectTypeComboBoxPrivate(qH5ObjectTypeComboBox &q)
    : q_ptr(&q) {}

qH5ObjectTypeComboBoxPrivate::~qH5ObjectTypeComboBoxPrivate() {}

void qH5ObjectTypeComboBoxPrivate::init() {
  Q_Q(qH5ObjectTypeComboBox);
  auto flags = magic_enum::enum_names<h5geo::ObjectType>();
  for (const auto &name : flags) {
    q->addItem(QString::fromStdString(std::string(name)));
  }
}

qH5ObjectTypeComboBox::qH5ObjectTypeComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5ObjectTypeComboBoxPrivate(*this)) {
  Q_D(qH5ObjectTypeComboBox);
  d->init();
}

qH5ObjectTypeComboBox::qH5ObjectTypeComboBox(qH5ObjectTypeComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5ObjectTypeComboBox::~qH5ObjectTypeComboBox() {

}

bool qH5ObjectTypeComboBox::setH5ObjectType(unsigned objectType)
{
  auto objectType_opt =
      magic_enum::enum_cast<h5geo::ObjectType>(objectType);

  if (!objectType_opt.has_value())
    return false;

  for (int i = 0; i < this->count(); i++){
    auto opt =
        magic_enum::enum_cast<h5geo::ObjectType>(
          this->itemText(i).toStdString());
    if (!opt.has_value())
      continue;

    if (opt.value() == objectType_opt.value()){
      this->setCurrentIndex(i);
      return true;
    }
  }
  return false;
}

unsigned qH5ObjectTypeComboBox::getH5ObjectType()
{
  auto objectType_opt =
      magic_enum::enum_cast<h5geo::ObjectType>(this->currentText().toStdString());
  if (objectType_opt.has_value())
    return static_cast<unsigned>(objectType_opt.value());

  return 0;
}

