// Colada includes
#include "qColadaH5FileDialog.h"
#include "qColadaH5FileDialog_p.h"
#include "qH5ContainerTypeComboBox.h"

// Qt includes
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>

// CTK includes
#include <ctkPathLineEdit.h>
#include <ctkFileDialog.h>

// h5geo includes
#include <h5geo/h5util.h>
#include <h5geo/h5basecontainer.h>

// magic_enum includes
#include <magic_enum.hpp>


qColadaH5FileDialogPrivate::qColadaH5FileDialogPrivate(qColadaH5FileDialog &q)
    : q_ptr(&q) {}

qColadaH5FileDialogPrivate::~qColadaH5FileDialogPrivate() {}

void qColadaH5FileDialogPrivate::init() {
  Q_Q(qColadaH5FileDialog);
  this->formLayout = new QFormLayout(q);

  this->containerTypeCombo = new qH5ContainerTypeComboBox(q);
  this->filePathLineEdit = new ctkPathLineEdit(q);
  this->fileCreateTypeCombo = new QComboBox(q);
  this->objNameLineEdit = new QLineEdit(q);
  this->objCreateTypeCombo = new QComboBox(q);

  this->filePathLineEdit->setFilters(
        ctkPathLineEdit::AllEntries |
        ctkPathLineEdit::Writable);
  this->filePathLineEdit->setOptions(
        ctkPathLineEdit::DontConfirmOverwrite |
        ctkPathLineEdit::DontUseNativeDialog);
  this->filePathLineEdit->setNameFilters(
        QStringList() <<
        "HDF5 (*.h5 *.hdf5)" <<
        "All Files (*)");

  auto creationTypes = magic_enum::enum_names<h5geo::CreationType>();
  QStringList creationTypeList;
  for (const auto &name : creationTypes) {
    creationTypeList.push_back(QString::fromStdString(std::string(name)));
  }

  this->fileCreateTypeCombo->addItems(creationTypeList);
  this->objCreateTypeCombo->addItems(creationTypeList);

  if (int defaultIndex = creationTypeList.indexOf("OPEN_OR_CREATE")){
    this->fileCreateTypeCombo->setCurrentIndex(defaultIndex);
    this->objCreateTypeCombo->setCurrentIndex(defaultIndex);
  }

  this->buttonBox = new QDialogButtonBox(Qt::Horizontal, q);
  this->buttonBox->setObjectName("ButtonBox");
  this->buttonBox->addButton(QDialogButtonBox::Ok);
  this->buttonBox->addButton(QDialogButtonBox::Cancel);

  this->formLayout->addRow("Container Type:", this->containerTypeCombo);
  this->formLayout->addRow("Container:", this->filePathLineEdit);
  this->formLayout->addRow("Container creation type:", this->fileCreateTypeCombo);
  this->formLayout->addRow("Object:", this->objNameLineEdit);
  this->formLayout->addRow("Object creation type:", this->objCreateTypeCombo);
  this->formLayout->addRow(this->buttonBox);

  QObject::connect(this->filePathLineEdit, &ctkPathLineEdit::currentPathChanged,
                   q, &qColadaH5FileDialog::onCurrentH5FilePathChanged);
  QObject::connect(this->buttonBox, &QDialogButtonBox::clicked,
                   q, &qColadaH5FileDialog::onButtonBoxClicked);
}

qColadaH5FileDialog::qColadaH5FileDialog(QWidget *parent)
    : QDialog(parent), d_ptr(new qColadaH5FileDialogPrivate(*this)) {
  Q_D(qColadaH5FileDialog);
  d->init();
}

qColadaH5FileDialog::qColadaH5FileDialog(qColadaH5FileDialogPrivate *pimpl, QWidget *parent)
    : QDialog(parent), d_ptr(pimpl) {
  // init() is called by derived class.
}

qColadaH5FileDialog::~qColadaH5FileDialog() {}

void qColadaH5FileDialog::onButtonBoxClicked(QAbstractButton *button)
{
  Q_D(qColadaH5FileDialog);

  if (button == d->buttonBox->button(QDialogButtonBox::Ok)) {
    this->accept(); // Hides the modal dialog and sets the result code to Accepted
  } else if (button == d->buttonBox->button(QDialogButtonBox::Cancel)) {
    this->reject(); // Hides the modal dialog and sets the result code to Rejected.
  }
}

qH5ContainerTypeComboBox* qColadaH5FileDialog::getH5ContainerTypeComboBox()
{
  Q_D(qColadaH5FileDialog);
  return d->containerTypeCombo;
}

ctkPathLineEdit* qColadaH5FileDialog::getH5FilePathLineEdit(){
  Q_D(qColadaH5FileDialog);
  return d->filePathLineEdit;
}

QLineEdit* qColadaH5FileDialog::getH5ObjectLineEdit(){
  Q_D(qColadaH5FileDialog);
  return d->objNameLineEdit;
}

QComboBox* qColadaH5FileDialog::getH5FileCreateTypeComboBox(){
  Q_D(qColadaH5FileDialog);
  return d->fileCreateTypeCombo;
}

QComboBox* qColadaH5FileDialog::getH5ObjectCreateTypeComboBox(){
  Q_D(qColadaH5FileDialog);
  return d->objCreateTypeCombo;
}

int qColadaH5FileDialog::getOpenH5FileDialog(
    QWidget *parent,
    QString& h5File,
    unsigned& containerType,
    unsigned& h5FileCreateType,
    QString& h5Obj,
    unsigned& h5ObjCreateType)
{
  qColadaH5FileDialog dialog(parent);
  if (dialog.exec() == QDialog::Rejected)
    return QDialog::Rejected;

  h5File = dialog.getH5FilePathLineEdit()->currentPath();
  containerType = static_cast<unsigned>(dialog.getH5ContainerTypeComboBox()->getH5ContainerType());
  auto h5FileCreateType_opt = magic_enum::enum_cast<h5geo::CreationType>(
        dialog.getH5FileCreateTypeComboBox()->currentText().toStdString());
  if (h5FileCreateType_opt)
    h5FileCreateType = static_cast<unsigned>(h5FileCreateType_opt.value());
  h5Obj = dialog.getH5ObjectLineEdit()->text();
  auto h5ObjectCreateType_opt = magic_enum::enum_cast<h5geo::CreationType>(
        dialog.getH5ObjectCreateTypeComboBox()->currentText().toStdString());
  if (h5ObjectCreateType_opt)
    h5ObjCreateType = static_cast<unsigned>(h5ObjectCreateType_opt.value());

  return QDialog::Accepted;
}

void qColadaH5FileDialog::onCurrentH5FilePathChanged(const QString& path)
{
  Q_D(qColadaH5FileDialog);
  H5BaseCnt_ptr cnt (h5geo::openBaseContainerByName(path.toStdString()));
  if (!cnt)
    return;

  d->containerTypeCombo->setH5ContainerType(
        static_cast<unsigned>(cnt->getContainerType()));
}
