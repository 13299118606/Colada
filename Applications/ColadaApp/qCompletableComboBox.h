#ifndef __qCompletableComboBox_h
#define __qCompletableComboBox_h

#include <QComboBox>

class qCompletableComboBox : public QComboBox
{
  Q_OBJECT
public:
  qCompletableComboBox(QWidget *parent = nullptr);

private slots:
  void onEditingFinished();
};

#endif
