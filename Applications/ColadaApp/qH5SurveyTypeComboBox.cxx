// Colada includes
#include "qH5SurveyTypeComboBox.h"
#include "qH5SurveyTypeComboBox_p.h"

// Qt includes
#include <QComboBox>
#include <QStringListModel>

// h5geo includes
#include <h5geo/h5util.h>

// magic_enum includes
#include <magic_enum.hpp>

qH5SurveyTypeComboBoxPrivate::qH5SurveyTypeComboBoxPrivate(qH5SurveyTypeComboBox &q)
    : q_ptr(&q) {}

qH5SurveyTypeComboBoxPrivate::~qH5SurveyTypeComboBoxPrivate() {}

void qH5SurveyTypeComboBoxPrivate::init() {
  Q_Q(qH5SurveyTypeComboBox);
  auto flags = magic_enum::enum_names<h5geo::SurveyType>();
  for (const auto &name : flags) {
    q->addItem(QString::fromStdString(std::string(name)));
  }
}

qH5SurveyTypeComboBox::qH5SurveyTypeComboBox(QWidget *parent)
  : QComboBox(parent), d_ptr(new qH5SurveyTypeComboBoxPrivate(*this)) {
  Q_D(qH5SurveyTypeComboBox);
  d->init();
}

qH5SurveyTypeComboBox::qH5SurveyTypeComboBox(qH5SurveyTypeComboBoxPrivate *pimpl, QWidget *parent)
    : QComboBox(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qH5SurveyTypeComboBox::~qH5SurveyTypeComboBox() {

}

bool qH5SurveyTypeComboBox::setH5SurveyType(unsigned survType){
  auto seisSurvType_opt =
      magic_enum::enum_cast<h5geo::SurveyType>(survType);

  if (!seisSurvType_opt.has_value())
    return false;

  for (int i = 0; i < this->count(); i++){
    auto opt =
        magic_enum::enum_cast<h5geo::SurveyType>(
          this->itemText(i).toStdString());
    if (!opt.has_value())
      continue;

    if (opt.value() == seisSurvType_opt.value()){
      this->setCurrentIndex(i);
      return true;
    }
  }
  return false;
}

unsigned qH5SurveyTypeComboBox::getH5SurveyType()
{
  auto seisSurvType_opt =
      magic_enum::enum_cast<h5geo::SurveyType>(this->currentText().toStdString());
  if (seisSurvType_opt.has_value())
    return static_cast<unsigned>(seisSurvType_opt.value());

  return 0;
}

