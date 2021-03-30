// Colada includes
#include "qColadaNewProject.h"

class qColadaNewProjectPrivate
{
  Q_DECLARE_PUBLIC(qColadaNewProject);
protected:
  qColadaNewProject* const q_ptr;

public:
  qColadaNewProjectPrivate(qColadaNewProject& object)
    : q_ptr(&object) {};

  void init() {};
  void setupUi(QDialog* coladaNewProject) {};
};

qColadaNewProject::qColadaNewProject(QWidget* parent) :
  QDialog(parent) {

}

qColadaNewProject::~qColadaNewProject() {

}