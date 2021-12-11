#ifndef __qMRMLCompletableNodeComboBox_h
#define __qMRMLCompletableNodeComboBox_h

#include <qMRMLNodeComboBox.h>
#include "qColadaAppExport.h"

class qMRMLCompletableNodeComboBoxPrivate;
class QComboBox;

class qMRMLCompletableNodeComboBox : public qMRMLNodeComboBox
{
  Q_OBJECT

public:
  typedef qMRMLNodeComboBox Superclass;
  explicit qMRMLCompletableNodeComboBox(QWidget *parent = nullptr);
  // qMRMLNodeComboBox has non-virtual destructor

  QComboBox* getComboBox();

protected slots:
  void onEditingFinished();

protected:
  explicit qMRMLCompletableNodeComboBox(
      qMRMLCompletableNodeComboBoxPrivate *pimpl,
      QWidget *parent);

private:
  Q_DECLARE_PRIVATE(qMRMLCompletableNodeComboBox);
  Q_DISABLE_COPY(qMRMLCompletableNodeComboBox);
};

#endif
