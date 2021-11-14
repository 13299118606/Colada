#ifndef __qColadaDarkStyle_h
#define __qColadaDarkStyle_h

// Slicer includes
#include "qColadaStyle.h"
#include "qColadaAppExport.h"

class Q_COLADA_APP_EXPORT qColadaDarkStyle : public qColadaStyle
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef qColadaStyle Superclass;

  /// Constructors
  qColadaDarkStyle();
  ~qColadaDarkStyle() override;

  QPalette standardPalette() const override;
};

#endif
