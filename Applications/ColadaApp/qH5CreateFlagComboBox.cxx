// Colada includes
#include "qH5CreateFlagComboBox.h"
#include "qH5CreateFlagComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5core.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5CreateFlagComboBoxPrivate::qH5CreateFlagComboBoxPrivate(qH5CreateFlagComboBox &q)
    : q_ptr(&q) {}

qH5CreateFlagComboBoxPrivate::~qH5CreateFlagComboBoxPrivate() {}

void qH5CreateFlagComboBoxPrivate::init() {
  Q_Q(qH5CreateFlagComboBox);
  q->setH5FlagsToAll();
}

qH5CreateFlagComboBox::qH5CreateFlagComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5CreateFlagComboBoxPrivate(*this)) {
  Q_D(qH5CreateFlagComboBox);
  d->init();
}

qH5CreateFlagComboBox::qH5CreateFlagComboBox(qH5CreateFlagComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5CreateFlagComboBox::~qH5CreateFlagComboBox() {

}

void qH5CreateFlagComboBox::setH5FlagsToOpen(){
  this->clear();
  this->addH5Flag(static_cast<unsigned>(h5geo::CreationType::OPEN));
  this->addH5Flag(static_cast<unsigned>(h5geo::CreationType::OPEN_OR_CREATE));
}

void qH5CreateFlagComboBox::setH5FlagsToCreate(){
  this->clear();
  this->addH5Flag(static_cast<unsigned>(h5geo::CreationType::CREATE));
  this->addH5Flag(static_cast<unsigned>(h5geo::CreationType::CREATE_OR_OVERWRITE));
  this->addH5Flag(static_cast<unsigned>(h5geo::CreationType::CREATE_UNDER_NEW_NAME));
}

void qH5CreateFlagComboBox::setH5FlagsToAll(){
  this->clear();
  auto flags = magic_enum::enum_names<h5geo::CreationType>();
  for (const auto &name : flags) {
    this->addItem(QString::fromStdString(std::string(name)));
  }
}

unsigned qH5CreateFlagComboBox::getH5CreateFlag()
{
  auto seisCreateType_opt =
      magic_enum::enum_cast<h5geo::CreationType>(this->currentText().toStdString());
  if (seisCreateType_opt.has_value())
    return static_cast<unsigned>(seisCreateType_opt.value());

  return 0;
}

void qH5CreateFlagComboBox::addH5Flag(unsigned flag){
  QString strFlag = QString::fromStdString(
        std::string{magic_enum::enum_name(
                    static_cast<h5geo::CreationType>(flag))});
  if (strFlag.isEmpty())
    return;

  int ind = this->findText(strFlag);
  if (ind > -1)
    return;

  this->addItem(strFlag);
}

void qH5CreateFlagComboBox::removeH5Flag(unsigned flag){
  QString strFlag = QString::fromStdString(
        std::string{magic_enum::enum_name(
                    static_cast<h5geo::CreationType>(flag))});
  if (strFlag.isEmpty())
    return;

  int ind = this->findText(strFlag);
  if (ind < 0)
    return;

  this->removeItem(ind);
}
