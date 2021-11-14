// Colada includes
#include "qColadaStylePlugin.h"
#include "qColadaStyle.h"
#include "qColadaDarkStyle.h"
#include "qColadaLightStyle.h"

// Qt includes
#include <QStringList>
#include <QStyle>


// --------------------------------------------------------------------------
// qColadaStylePlugin methods

//-----------------------------------------------------------------------------
qColadaStylePlugin::qColadaStylePlugin() = default;

//-----------------------------------------------------------------------------
qColadaStylePlugin::~qColadaStylePlugin() = default;

//-----------------------------------------------------------------------------
QStyle* qColadaStylePlugin::create( const QString & key )
{
  if (key.compare("Colada", Qt::CaseInsensitive) == 0)
    {
    return new qColadaStyle();
    }
  if (key.compare("Light Colada", Qt::CaseInsensitive) == 0)
    {
    return new qColadaLightStyle();
    }
  if (key.compare("Dark Colada", Qt::CaseInsensitive) == 0)
    {
    return new qColadaDarkStyle();
    }
  return nullptr;
}

//-----------------------------------------------------------------------------
QStringList qColadaStylePlugin::keys() const
{
  return QStringList() << "Colada" << "Light Colada" << "Dark Colada";
}

