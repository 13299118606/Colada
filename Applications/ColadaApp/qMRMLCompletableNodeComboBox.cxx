// qMRML includes
#include "qMRMLCompletableNodeComboBox.h"
#include "qMRMLCompletableNodeComboBox_p.h"
#include "qMRMLSceneModel.h"

#include <QLineEdit>
#include <QCompleter>

#include "ctkSearchBox.h"

qMRMLCompletableNodeComboBoxPrivate::qMRMLCompletableNodeComboBoxPrivate(
    qMRMLCompletableNodeComboBox &q)
    : Superclass(q) {}

qMRMLCompletableNodeComboBoxPrivate::~qMRMLCompletableNodeComboBoxPrivate() {}

void qMRMLCompletableNodeComboBoxPrivate::init() {
  Q_Q(qMRMLCompletableNodeComboBox);
  // don't need to call Superclass::init() as it is called independently in this case

  ComboBox->setEditable(true);
  ctkSearchBox* customLineEdit = new ctkSearchBox(q);
  customLineEdit->setReadOnly(false);
  ComboBox->setLineEdit(customLineEdit);

  ComboBox->completer()->setCompletionMode(QCompleter::PopupCompletion);
  ComboBox->completer()->setCaseSensitivity(Qt::CaseInsensitive);

  q->connect(ComboBox->lineEdit(), &QLineEdit::editingFinished,
             q, &qMRMLCompletableNodeComboBox::onEditingFinished);
}

qMRMLCompletableNodeComboBox::qMRMLCompletableNodeComboBox(QWidget *parent)
    : qMRMLNodeComboBox(new qMRMLCompletableNodeComboBoxPrivate(*this), parent) {
  Q_D(qMRMLCompletableNodeComboBox);
  d->init();
}

qMRMLCompletableNodeComboBox::qMRMLCompletableNodeComboBox(
    qMRMLCompletableNodeComboBoxPrivate *pimpl, QWidget *parent)
    : qMRMLNodeComboBox(pimpl, parent) {
  // init() is called by derived class.
}

void qMRMLCompletableNodeComboBox::onEditingFinished(){
  Q_D(qMRMLCompletableNodeComboBox);
  qMRMLSceneModel* sceneModel = qobject_cast<qMRMLSceneModel*>(model());
  if (!sceneModel)
    return;

  QModelIndexList indexList = sceneModel->match(
        sceneModel->index(0,0),
        Qt::DisplayRole,
        d->ComboBox->currentText());

  if (!indexList.isEmpty())
    d->ComboBox->setCurrentIndex(indexList[0].row());
  else
    d->ComboBox->setCurrentIndex(d->ComboBox->currentIndex());
}

QComboBox* qMRMLCompletableNodeComboBox::getComboBox()
{
  Q_D(qMRMLCompletableNodeComboBox);
  return d->ComboBox;
}
