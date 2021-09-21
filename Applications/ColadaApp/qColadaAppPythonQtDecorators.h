#ifndef __qColadaAppPythonQtDecorators_h
#define __qColadaAppPythonQtDecorators_h

#include "PythonQt.h"
#include "PythonQtCppWrapperFactory.h"

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkCheckableHeaderView.h>

// Colada includes
#include "qColadaAppExport.h"

/* Rules to create bindings via PythonQt:
   - don`t write `~MyClass = default`;
   - don`t use EXPORT directive for a decorator class;
   - use name conventions when declaring methods of decorator`s 
   class to overload constructor or add static methods (new_MyClass or static_MyMethod)
   - `PythonQt::self()->addDecorators(...)` automatically adds `registerClass` but
   to set your own module name you should better manually write `PythonQt::self()->registerClass(...)`
   - all decorator staff and its initialization implement in HEADER file or you will get multiple function definition error
   - see more info at: https://doc.qt.io/archives/qq/qq23-pythonqt.html#:~:text=a%20%3D%20QSize(1%2C2)-,Decorators%20and%20C%2B%2B%20Wrappers,-A%20major%20problem
*/

class qColadaAppPythonQtDecorators : public QObject {
  Q_OBJECT
//
public:
//  explicit qColadaAppPythonQtDecorators(Qt::Orientation orient, QWidget *parent = nullptr);
  explicit qColadaAppPythonQtDecorators(QObject *parent = nullptr): 
    QObject(parent) {
    PythonQt::self()->registerClass(&ctkCheckableHeaderView::staticMetaObject,
                                    "qColadaApp");
    //PythonQt::self()->registerCPPClass(
    //    "ctkCheckableHeaderView", "", "qColadaAppPythonQt",
    //    PythonQtCreateObject<ctkCheckableHeaderView(Qt::Vertical)>);


    //PythonQt::self()->registerCPPClass("ctkErrorLogContext", 0, "CTKCore");
    //PythonQt::self()->registerCPPClass("ctkWorkflowStep", 0, "CTKCore");
    //PythonQt::self()->registerClass(
    //    &ctkWorkflowInterstepTransition::staticMetaObject, "CTKCore");
  }

public slots:
  ctkCheckableHeaderView *new_ctkCheckableHeaderView(Qt::Orientation orient, QWidget *parent = nullptr) {
    return new ctkCheckableHeaderView(orient, parent);
  }

  //Qt::CheckState checkState(ctkCheckableHeaderView *o, int section) { 
  //  return o->checkState(section);
  //}
};

void initqColadaAppPythonQtDecorators() {
  PythonQt::self()->addDecorators(new qColadaAppPythonQtDecorators);

  // PythonQt doesn't support wrapping a static function and adding it to the
  // top-level ctk module. This exposes static functions from ctkCoreUtils as
  // ctk.ctkCoreUtils.absolutePathFromInternal(), etc. Note that
  // PythonQtWrapper_CTKCore installs itself as ctk.ctk but using that same
  // module here would replace PythonQtWrapper_CTKCore.
  // PythonQt::self()->registerCPPClass("qColadaAppPythonQtDecorators", "",
  // "qColadaAppPythonQt",
  //                                   PythonQtCreateObject<qColadaAppPythonQtDecorators>);
}

#endif
