#ifndef __TEST_h
#define __TEST_h

// Qt includes
#include <QTreeView>

// CTK includes
#include <ctkCheckableHeaderView.h>

// Colada includes
#include "qColadaAppExport.h"

// h5gt includes
//#include <h5gt/H5File.hpp>

#include <PythonQt.h>

class TEST : public QObject {
  Q_OBJECT

//public:
  //explicit TEST(Qt::Orientation orient, QWidget *parent = nullptr);
  //~TEST() = default;

//public slots:
//  bool addH5File(h5gt::File file);

public slots:
  ctkCheckableHeaderView *
  new_ctkCheckableHeaderView(Qt::Orientation orient,
                             QWidget *parent = nullptr) {
    return new ctkCheckableHeaderView(orient, parent);
  }

  Qt::CheckState checkState(ctkCheckableHeaderView *o, int section) { 
    return o->checkState(section);
  }
};

//void initH5GT_PythonQt();


#endif
