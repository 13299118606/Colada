#ifndef __qColadaNewProject_h
#define __qColadaNewProject_h

// Qt includes
#include <QDialog>

// Colada includes
#include "qColadaAppExport.h"
class qColadaNewProjectPrivate;

class Q_COLADA_APP_EXPORT qColadaNewProject : public QDialog
{
  Q_OBJECT

public:
  qColadaNewProject(QWidget* parentWidget = nullptr);
  ~qColadaNewProject() override;

protected:
  QScopedPointer<qColadaNewProjectPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaNewProject);
  Q_DISABLE_COPY(qColadaNewProject);
};

#endif
