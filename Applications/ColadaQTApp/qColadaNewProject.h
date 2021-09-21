#ifndef __qColadaNewProject_h
#define __qColadaNewProject_h

// Qt includes
#include <QDialog>

// Colada includes
#include "qColadaAppExport.h"
class qColadaNewProjectPrivate;
class QAbstractButton;

class Q_COLADA_APP_EXPORT qColadaNewProject : public QDialog
{
	Q_OBJECT

private:
	struct Param {
		QString owner,
			prjName,
			prjDir,
			crsAuthName,
			crsCode,
			crsName;
	};

	qColadaNewProject::Param getParam();

public:
	qColadaNewProject(QWidget* parentWidget = nullptr);
  virtual ~qColadaNewProject() override;

public slots:
	void onCrsCheckBoxStateChanged(int state);
	void onButtonBoxClicked(QAbstractButton* button);

protected:
	QScopedPointer<qColadaNewProjectPrivate> d_ptr;

private:
	Q_DECLARE_PRIVATE(qColadaNewProject);
	Q_DISABLE_COPY(qColadaNewProject);
};

#endif
