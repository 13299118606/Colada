// Qt includes
#include <QLinearGradient>
#include <QPalette>

// qMRML includes
#include "qColadaLightStyle.h"

// --------------------------------------------------------------------------
// qColadaStyle methods

// --------------------------------------------------------------------------
qColadaLightStyle::qColadaLightStyle() = default;

// --------------------------------------------------------------------------
qColadaLightStyle::~qColadaLightStyle() = default;

//------------------------------------------------------------------------------
QPalette qColadaLightStyle::standardPalette()const
{
  QPalette palette = this->Superclass::standardLightPalette();
  return palette;
}
