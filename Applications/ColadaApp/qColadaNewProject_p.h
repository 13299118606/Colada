#ifndef __qColadaNewProject_p_h
#define __qColadaNewProject_p_h

// Colada includes
#include "qColadaNewProject.h"

class QLineEdit;
class ctkPathLineEdit;
class QCheckBox;
class qCRSWidget;
class QDialogButtonBox;

class Q_COLADA_APP_EXPORT qColadaNewProjectPrivate
{
  Q_DECLARE_PUBLIC(qColadaNewProject);

protected:
  qColadaNewProject* const q_ptr;

public:
  typedef qColadaNewProjectPrivate Superclass;
  qColadaNewProjectPrivate(qColadaNewProject& q);
  virtual ~qColadaNewProjectPrivate();

  virtual void init();
  virtual void setupUi(QDialog* q);

public:
  QLineEdit* ownerLineEdit, * prjNameLineEdit;
  ctkPathLineEdit* pathCtkLineEdit;
  QCheckBox* crsCheckBox;
  qCRSWidget* crsWidget;
  QDialogButtonBox* buttonBox;
};

#endif