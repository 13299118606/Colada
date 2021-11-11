#ifndef __qColadaApplication_h
#define __qColadaApplication_h

// CTK includes
#include <ctkPimpl.h>
#include <ctkSettingsDialog.h>

// QTGUI includes
#include "qSlicerApplication.h"

// Colada includes
#include "qColadaAppExport.h"

class qColadaApplicationPrivate;

class Q_COLADA_APP_EXPORT qColadaApplication : public qSlicerApplication
{
  Q_OBJECT
public:

  typedef qSlicerApplication Superclass;
  qColadaApplication(int &argc, char **argv);
  virtual ~qColadaApplication();

  /// Return a reference to the application singleton
  static qColadaApplication* application();

  QString getCRSName();

public slots:

protected:
  qColadaApplication(qColadaApplicationPrivate* pimpl, int &argc, char **argv);
  QScopedPointer<qColadaApplicationPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qColadaApplication);
  Q_DISABLE_COPY(qColadaApplication);
};

#endif
