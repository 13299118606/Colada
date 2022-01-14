
#-----------------------------------------------------------------------------
set(MODULE_NAME SpatialReference)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
# h5geo requires its deps to be found first
find_package(OpenMP REQUIRED) # Colada needs OpenMP
find_package(ZLIB REQUIRED)
find_package(units REQUIRED CONFIG)  # based on units_DIR
find_package(HDF5 REQUIRED CONFIG)  # based on HDF5_DIR
find_package(GDAL REQUIRED)
find_package(h5geo REQUIRED CONFIG)  # based on h5geo_DIR

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${qColadaApp_INCLUDE_DIRS}
  ${qSlicerUnitsModuleWidgets_INCLUDE_DIRS}
  ${Eigen3_INCLUDE_DIR}
  ${h5gt_INCLUDE_DIR}
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}SettingsPanel.cxx
  qSlicer${MODULE_NAME}SettingsPanel.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}SettingsPanel.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}SettingsPanel.ui
  )

set(MODULE_TARGET_LIBRARIES
  qColadaApp
  qSlicerUnitsModuleWidgets
  h5geo
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildLoadableModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
#  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
#if(BUILD_TESTING)
#  add_subdirectory(Testing)
#endif()
