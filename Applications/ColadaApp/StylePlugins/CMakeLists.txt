project(qColadaAppStylePlugins)
set(KIT ${PROJECT_NAME})

#
# See CTK/CMake/ctkMacroBuildQtPlugin.cmake for details
#

# The header '${KIT}Export.h' will be automatically configured.
set(${KIT}_EXPORT "Q_COLADA_STYLES_PLUGINS_EXPORT")

# Source files
set(${KIT}_SRCS
  qColadaStylePlugin.cxx
  qColadaStylePlugin.h
  )

# Headers that should run through moc
set(${KIT}_MOC_SRCS
  qColadaStylePlugin.h
  )

set(${KIT}_TARGET_LIBRARIES
  qSlicerBaseQTGUI
  qColadaApp
  )

ctkMacroBuildQtStylesPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT}
  FOLDER "Colada-Base"
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )