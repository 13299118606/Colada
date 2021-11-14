// Qt includes
#include <QLinearGradient>
#include <QPalette>

// qMRML includes
#include "qColadaDarkStyle.h"

// --------------------------------------------------------------------------
// qColadaStyle methods

// --------------------------------------------------------------------------
qColadaDarkStyle::qColadaDarkStyle() = default;

// --------------------------------------------------------------------------
qColadaDarkStyle::~qColadaDarkStyle() = default;

//------------------------------------------------------------------------------
QPalette qColadaDarkStyle::standardPalette()const
{
  QPalette palette = this->Superclass::standardDarkPalette();
  return palette;
}
