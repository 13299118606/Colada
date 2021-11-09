#include "qCompletableComboBox.h"

#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>

#include "ctkSearchBox.h"

qCompletableComboBox::qCompletableComboBox(QWidget *parent) :
    QComboBox(parent)
{
  setEditable(true);
  ctkSearchBox* customLineEdit = new ctkSearchBox(this);
  setLineEdit(customLineEdit);

  completer()->setCompletionMode(QCompleter::PopupCompletion);
  completer()->setCaseSensitivity(Qt::CaseInsensitive);

  connect(lineEdit(), &QLineEdit::editingFinished,
          this, &qCompletableComboBox::onEditingFinished);
}

void qCompletableComboBox::onEditingFinished(){
  QStringListModel* stringListModel = qobject_cast<QStringListModel*>(model());
  if (!stringListModel)
    return;

  int ind = stringListModel->stringList().indexOf(
            QRegExp(currentText(), Qt::CaseInsensitive, QRegExp::Wildcard), 0);

  if (ind >= 0 && ind < stringListModel->stringList().count()){
    setCurrentIndex(ind);
  } else {
    setCurrentIndex(currentIndex());
  }
}
