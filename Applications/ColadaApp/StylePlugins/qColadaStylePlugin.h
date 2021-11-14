#ifndef __qColadaStylePlugin_h
#define __qColadaStylePlugin_h

// Qt includes
#include <QStylePlugin>
#include <QtPlugin>

class QStyle;

// Colada includes
#include "qColadaAppStylePluginsExport.h"

class Q_COLADA_STYLES_PLUGINS_EXPORT qColadaStylePlugin : public QStylePlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QStyleFactoryInterface_iid FILE "ColadaStyle.json")
public:
  /// Superclass typedef
  typedef QStylePlugin Superclass;

  /// Constructors
  qColadaStylePlugin();
  ~qColadaStylePlugin() override;

  // QStyle plugin classes to overloaded when creating custom style plugins
  QStyle* create(const QString & key) override;
  virtual QStringList keys() const;
};

#endif
