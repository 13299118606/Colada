// Colada includes
#include "qColadaNewProject.h"
#include "qColadaNewProject_p.h"
#include "qCRSWidget.h"
#include "qREValidator.h"
#include "../Core/ColadaUtil.h"
#include "../Core/ColadaDBCore.h"

// Qt includes
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QRegularExpression>
#include <QTableView>
#include <QTextBrowser>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QSqlDatabase>

// CTK includes
#include <ctkPathLineEdit.h>
#include <ctkSearchBox.h>

qColadaNewProjectPrivate::qColadaNewProjectPrivate(qColadaNewProject& q)
  : q_ptr(&q)
{

}

qColadaNewProjectPrivate::~qColadaNewProjectPrivate() {

}

void qColadaNewProjectPrivate::init() {
  Q_Q(qColadaNewProject);
  q->setWindowTitle("New project");
  q->setWindowFlags(
      q->windowFlags() &
      ~Qt::WindowContextHelpButtonHint & // removes question '?' sign
      Qt::WindowMaximizeButtonHint);     // add fullscreen button
  q->setAttribute(
      Qt::WA_DeleteOnClose); // will delete the dialog when it is closed
  q->setModal(true);
  this->setupUi(q);
}

void qColadaNewProjectPrivate::setupUi(QDialog* q) {
  ownerLineEdit = new QLineEdit();
  /* filename characters accepted only except space */
  QRegularExpression re = util::fileNameRegExp();
  qREValidator* ownerValidator = new qREValidator(
    re, util::fileNameNoSpaceToolTipText(), 3000, ownerLineEdit);
  ownerLineEdit->setValidator(ownerValidator);
  prjNameLineEdit = new QLineEdit();
  /* filename characters accepted only */
  qREValidator* prjNameValidator = new qREValidator(
    re, util::fileNameToolTipText(), 3000, ownerLineEdit);
  prjNameLineEdit->setValidator(prjNameValidator);
  pathCtkLineEdit = new ctkPathLineEdit();
  pathCtkLineEdit->setFilters(ctkPathLineEdit::AllDirs);

  QFormLayout* formLayout = new QFormLayout();
  formLayout->addRow("Owner:", ownerLineEdit);
  formLayout->addRow("Project name:", prjNameLineEdit);
  formLayout->addRow("Project path:", pathCtkLineEdit);

  buttonBox = new QDialogButtonBox(Qt::Horizontal);
  buttonBox->addButton(QDialogButtonBox::Ok);
  buttonBox->addButton(QDialogButtonBox::Cancel);

  crsCheckBox = new QCheckBox("No projection");
  crsWidget = new qCRSWidget();

  QVBoxLayout* mainLayout = new QVBoxLayout(q);
  mainLayout->addLayout(formLayout, 0);
  mainLayout->addWidget(crsCheckBox, 0);
  mainLayout->addWidget(crsWidget, 1);
  mainLayout->addWidget(buttonBox, 0);
}

qColadaNewProject::qColadaNewProject(QWidget* parent) :
  QDialog(parent), d_ptr(new qColadaNewProjectPrivate(*this))
{
  Q_D(qColadaNewProject);
  d->init();

  connect(d->crsCheckBox, &QCheckBox::stateChanged, this, &qColadaNewProject::onCrsCheckBoxStateChanged);
  connect(d->buttonBox, &QDialogButtonBox::clicked, this, &qColadaNewProject::onButtonBoxClicked);
}

qColadaNewProject::~qColadaNewProject() {

}

qColadaNewProject::Param qColadaNewProject::getParam() {
  Q_D(qColadaNewProject);

  Param p;
  p.owner = d->ownerLineEdit->text();
  p.prjName = d->prjNameLineEdit->text();
  p.prjDir = d->pathCtkLineEdit->currentPath();

  QItemSelectionModel* selectionModel = d->crsWidget->getTableView()->selectionModel();
  QModelIndexList indexList = selectionModel->selectedRows();
  if (indexList.isEmpty())
    return p;

  int row = indexList[0].row();

  p.crsName = d->crsWidget->getProxyModel()->data(
    d->crsWidget->getProxyModel()->index(row, 0)).toString();
  p.crsAuthName = d->crsWidget->getProxyModel()->data(
    d->crsWidget->getProxyModel()->index(row, 1)).toString();
  p.crsCode = d->crsWidget->getProxyModel()->data(
    d->crsWidget->getProxyModel()->index(row, 2)).toString();

  return p;
}

void qColadaNewProject::onCrsCheckBoxStateChanged(int state) {
  Q_D(qColadaNewProject);
  if (state == Qt::Unchecked) {
    d->crsWidget->getSearchBox()->setEnabled(true);
    d->crsWidget->getTableView()->setEnabled(true);
    d->crsWidget->getTextBrowser()->setEnabled(true);
  }
  else {
    d->crsWidget->getSearchBox()->setDisabled(true);
    d->crsWidget->getTableView()->setDisabled(true);
    d->crsWidget->getTextBrowser()->setDisabled(true);
  }
}

void qColadaNewProject::onButtonBoxClicked(QAbstractButton* button) {
  Q_D(qColadaNewProject);

  if (button == d->buttonBox->button(QDialogButtonBox::Ok)) {
    Param p = getParam();

    if (p.owner.isEmpty() ||
      p.prjName.isEmpty() ||
      p.prjDir.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Some fields are empty"));
      msgBox.exec();
      return;
    }
    if (!d->crsCheckBox->isChecked() &&
      p.crsName.isEmpty() &&
      p.crsAuthName.isEmpty() &&
      p.crsCode.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Projected CRS not chosen"));
      msgBox.exec();
      return;
    }

    if (d->crsCheckBox->isChecked()) {
      p.crsName = "";
      p.crsAuthName = "";
      p.crsCode = "";
    }

    QDir qDir(p.prjDir + "/" + p.prjName);
    if (!qDir.mkpath(qDir.absolutePath())) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create project dir: ") + p.prjDir + "/" + p.prjName);
      msgBox.exec();
      return;
    }

    if (!qDir.isEmpty()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(qDir.absolutePath() +
        tr(" is not empty.\n") +
        tr("Clean it up and try again\n"));
      msgBox.exec();
      return;
    }

    QSqlDatabase db = dbcore::createDB(
      qDir.absolutePath(), p.prjName);
    if (!db.open()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't open: ") +
        db.databaseName());
      msgBox.exec();
      return;
    }
    if (!dbcore::createInfoTable()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create 'info' table"));
      msgBox.exec();
      return;
    }
    if (!dbcore::createSeisTable()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create 'seis_containers' table"));
      msgBox.exec();
      return;
    }
    if (!dbcore::createHrzTable()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create 'hrz_containers' table"));
      msgBox.exec();
      return;
    }
    if (!dbcore::createWellTable()) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create 'well_containers' table"));
      msgBox.exec();
      return;
    }
    if (!dbcore::createPrjFolders(qDir.absolutePath())) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't create project folders"));
      msgBox.exec();
      return;
    }

    if (!dbcore::fillInfoTable(p.owner,
      p.prjName,
      p.prjDir,
      p.crsAuthName,
      p.crsCode,
      p.crsName)) {
      QMessageBox msgBox;
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.setText(tr("Can't fill 'info' table"));
      msgBox.exec();
      return;
    }

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(db.databaseName() + " is created");
    msgBox.exec();
    this->close(); // close this widget
  }
  else if (button == d->buttonBox->button(QDialogButtonBox::Cancel)) {
    this->close(); // close this widget
  }
}
