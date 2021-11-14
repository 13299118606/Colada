#ifndef __qColadaLightStyle_h
#define __qColadaLightStyle_h

// Slicer includes
#include "qColadaStyle.h"
#include "qColadaAppExport.h"

class Q_COLADA_APP_EXPORT qColadaLightStyle : public qColadaStyle
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef qColadaStyle Superclass;

  /// Constructors
  qColadaLightStyle();
  ~qColadaLightStyle() override;

  QPalette standardPalette() const override;
};

#endif
