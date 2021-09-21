project(ColadaQTApp)

#
# See CMake/SlicerMacroBuildBaseQtLibrary.cmake for details
#

set(KIT_export_directive "Q_SLICER_BASE_QTAPP_EXPORT")

# Additional directories to include
set(KIT_include_directories
  ${qSlicerBaseQTCore_SOURCE_DIR}
  ${qSlicerBaseQTCore_BINARY_DIR}
  ${qSlicerBaseQTGUI_SOURCE_DIR}
  ${qSlicerBaseQTGUI_BINARY_DIR}
  ${qSlicerModulesCore_SOURCE_DIR}
  ${qSlicerModulesCore_BINARY_DIR}
  )

# Source files
set(KIT_SRCS
  qSlicerAboutDialog.cxx
  qSlicerAboutDialog.h
  qSlicerApplicationHelper.cxx
  qSlicerApplicationHelper.h
  qSlicerErrorReportDialog.cxx
  qSlicerErrorReportDialog.h
  qSlicerMainWindow.cxx
  qSlicerMainWindow.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  qSlicerAboutDialog.h
  qSlicerApplicationHelper.h
  qSlicerErrorReportDialog.h
  qSlicerMainWindow.h
  )

# UI files
set(KIT_UI_SRCS
  Resources/UI/qSlicerAboutDialog.ui
  Resources/UI/qSlicerErrorReportDialog.ui
  Resources/UI/qSlicerMainWindow.ui
  )








include(SlicerMacroBuildApplication)

# --------------------------------------------------------------------------
# Properties
# --------------------------------------------------------------------------
SlicerReadApplicationProperties()

# --------------------------------------------------------------------------
# Folder
# --------------------------------------------------------------------------
set(${PROJECT_NAME}_FOLDER "App-${PROJECT_NAME}")

find_package(ZLIB REQUIRED)
find_package(HDF5 REQUIRED CONFIG)  # based on HDF5_DIR
find_package(h5geo REQUIRED CONFIG)  # based on h5geo_DIR
find_package(GDAL REQUIRED)

# GDAL::GDAL depends on many libs (TIFF, GEOS etc). Here we are linking them
if(DEFINED GDAL_LIBS)
  target_link_libraries(GDAL::GDAL INTERFACE ${GDAL_LIBS})
endif()

# --------------------------------------------------------------------------
# Application library
# --------------------------------------------------------------------------
set(APPLIB_NAME "q${PROJECT_NAME}")

set(APPLIB_SRCS
  qColadaAppMainWindow.cxx
  qColadaAppMainWindow.h

  Core/ColadaDBCore.cxx
  Core/ColadaDBCore.h
  Core/ColadaUtil.cxx
  Core/ColadaUtil.h
  
  Widgets/qAppStyle.cxx
  Widgets/qAppStyle.h
  Widgets/qColadaNewProject.cxx
  Widgets/qColadaNewProject.h
  Widgets/qCRSDropTableView.h
  Widgets/qCRSDropTableView.cxx
  Widgets/qCRSWidget.cxx
  Widgets/qCRSWidget.h
  Widgets/qREValidator.cxx
  Widgets/qREValidator.h
  Widgets/qColadaH5TreeView.cxx
  Widgets/qColadaH5TreeView.h
  Widgets/qColadaH5SeisTreeView.cxx
  Widgets/qColadaH5SeisTreeView.h
  Widgets/qColadaH5SurfTreeView.cxx
  Widgets/qColadaH5SurfTreeView.h
  Widgets/qColadaH5WellTreeView.cxx
  Widgets/qColadaH5WellTreeView.h
  Widgets/qColadaH5Item.cxx
  Widgets/qColadaH5Item.h
  Widgets/qColadaH5ItemDelegate.cxx
  Widgets/qColadaH5ItemDelegate.h
  Widgets/qColadaH5Model.cxx
  Widgets/qColadaH5Model.h
  Widgets/qColadaH5SeisModel.cxx
  Widgets/qColadaH5SeisModel.h
  Widgets/qColadaH5SurfModel.cxx
  Widgets/qColadaH5SurfModel.h
  Widgets/qColadaH5WellModel.cxx
  Widgets/qColadaH5WellModel.h
  Widgets/qColadaH5ProxyModel.cxx
  Widgets/qColadaH5ProxyModel.h
  Widgets/qColadaReader.h
  Widgets/qColadaReader.cxx
  Widgets/qCopyPasteTableView.h
  Widgets/qCopyPasteTableView.cxx
  Widgets/qScienceSpinBox.h
  Widgets/qScienceSpinBox.cxx
  Widgets/qColadaSegyReader.h
  Widgets/qColadaSegyReader.cxx
  Widgets/qComboBoxDelegate.h
  Widgets/qComboBoxDelegate.cxx
  Widgets/qSpinBoxDelegate.h
  Widgets/qSpinBoxDelegate.cxx
  Widgets/qScienceSpinBoxDelegate.h
  Widgets/qScienceSpinBoxDelegate.cxx
  Widgets/qLineEditDelegate.h
  Widgets/qLineEditDelegate.cxx
  Widgets/qPathEditDelegate.h
  Widgets/qPathEditDelegate.cxx
  )

set(APPLIB_MOC_SRCS
  qColadaAppMainWindow.h
  Widgets/qAppStyle.h
  Widgets/qColadaNewProject.h
  Widgets/qCRSDropTableView.h
  Widgets/qCRSWidget.h
  Widgets/qREValidator.h
  Widgets/qColadaH5TreeView.h
  Widgets/qColadaH5SurfTreeView.h
  Widgets/qColadaH5SeisTreeView.h
  Widgets/qColadaH5WellTreeView.h
  Widgets/qColadaH5ItemDelegate.h
  Widgets/qColadaH5Model.h
  Widgets/qColadaH5SurfModel.h
  Widgets/qColadaH5SeisModel.h
  Widgets/qColadaH5WellModel.h
  Widgets/qColadaH5ProxyModel.h
  Widgets/qColadaReader.h
  Widgets/qCopyPasteTableView.h
  Widgets/qScienceSpinBox.h
  Widgets/qColadaSegyReader.h
  Widgets/qComboBoxDelegate.h
  Widgets/qSpinBoxDelegate.h
  Widgets/qScienceSpinBoxDelegate.h
  Widgets/qLineEditDelegate.h
  Widgets/qPathEditDelegate.h
  )

set(APPLIB_UI_SRCS
  )

set(APPLIB_RESOURCES
  Resources/App.qrc
  )

set(APPLIB_TARGET_LIBRARIES 
  # ColadaCore
  hdf5::hdf5-shared
  h5geo
  GDAL::GDAL
  ${julia_LIB}
  )

if(WIN32)
  list(APPEND APPLIB_TARGET_LIBRARIES ${julia_openlibm_LIB})
endif()

# Sanity checks
set(include_dirs
  ${Eigen3_INCLUDE_DIR}
  ${h5gt_INCLUDE_DIR}
  ${magic_enum_INCLUDE_DIR}
  ${julia_INCLUDE_DIR}
  )

foreach(var ${include_dirs})
  if(NOT EXISTS "${var}")
    message(FATAL_ERROR "Path to include dir: ${var} doesn't exist!")
  endif()
endforeach()

set(APPLIB_DIRS 
  ${Eigen3_INCLUDE_DIR}
  ${h5gt_INCLUDE_DIR}
  ${magic_enum_INCLUDE_DIR}
  ${julia_INCLUDE_DIR}
  )

slicerMacroBuildAppLibrary(
  NAME ${APPLIB_NAME}
  DESCRIPTION_SUMMARY ${${PROJECT_NAME}_DESCRIPTION_SUMMARY}
  DESCRIPTION_FILE ${${PROJECT_NAME}_DESCRIPTION_FILE}
  APPLICATION_NAME ${${PROJECT_NAME}_APPLICATION_NAME}
  EXPORT_DIRECTIVE "Q_COLADA_APP_EXPORT"
  FOLDER ${${PROJECT_NAME}_FOLDER}
  SRCS ${APPLIB_SRCS}
  MOC_SRCS ${APPLIB_MOC_SRCS}
  UI_SRCS ${APPLIB_UI_SRCS}
  RESOURCES ${APPLIB_RESOURCES}
  WRAP_PYTHONQT
  TARGET_LIBRARIES ${APPLIB_TARGET_LIBRARIES}
  INCLUDE_DIRECTORIES ${APPLIB_DIRS}
  )

# --------------------------------------------------------------------------
# Application executable
# --------------------------------------------------------------------------

# Configure launcher only for the main application
set(extra_args)
if(${PROJECT_NAME} STREQUAL ${Slicer_MAIN_PROJECT})
  set(extra_args CONFIGURE_LAUNCHER)
endif()

set(APP_SRCS
  Main.cxx
  )

set(APP_TARGET_LIBRARIES ${APPLIB_NAME})

slicerMacroBuildApplication(
  NAME ${PROJECT_NAME}
  APPLICATION_NAME ${${PROJECT_NAME}_APPLICATION_NAME}
  LAUNCHER_SPLASHSCREEN_FILE ${${PROJECT_NAME}_LAUNCHER_SPLASHSCREEN_FILE}
  APPLE_ICON_FILE ${${PROJECT_NAME}_APPLE_ICON_FILE}
  WIN_ICON_FILE ${${PROJECT_NAME}_WIN_ICON_FILE}
  LICENSE_FILE ${${PROJECT_NAME}_LICENSE_FILE}
  FOLDER ${${PROJECT_NAME}_FOLDER}
  SRCS ${APP_SRCS}
  TARGET_LIBRARIES ${APP_TARGET_LIBRARIES}
  TARGET_NAME_VAR "APP_TARGET_NAME"
  DEFAULT_SETTINGS_FILE Resources/Settings/DefaultSettings.ini
  ${extra_args}
  )

#-----------------------------------------------------------------------------
message("Slicer_INSTALL_ROOT: ${Slicer_INSTALL_ROOT}")
message("Slicer_INSTALL_BIN_DIR: ${Slicer_INSTALL_BIN_DIR}")
message("julia_ROOT: ${julia_ROOT}")
# Install extension deps packages
# install(CODE "message(\"CPack: - Install directory: ${julia_ROOT}\")")
install(
  DIRECTORY ${julia_ROOT}/  # '/' in the end is necessary
  DESTINATION ${Slicer_INSTALL_ROOT}/julia
  COMPONENT RuntimeLibraries  # 'ALL' doesn't work
  )

# Install libraries
include(${Slicer_SOURCE_DIR}/CMake/SlicerFunctionInstallLibrary.cmake)
slicerInstallLibrary(
  FILE ${HDF5_ROOT}/lib/hdf5
  DESTINATION ${Slicer_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )
slicerInstallLibrary(
  FILE ${h5geo_ROOT}/lib/h5geo
  DESTINATION ${Slicer_INSTALL_BIN_DIR}
  COMPONENT RuntimeLibraries
  )

set(EXTENSION_NAME Colada)
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${HDF5_ROOT};HDF5;ALL;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${h5geo_ROOT};h5geo;ALL;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${julia_ROOT};julia;ALL;/")

#-----------------------------------------------------------------------------
# list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
# list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
# include(${Slicer_EXTENSION_GENERATE_CONFIG})
# include(${Slicer_SOURCE_DIR}/CMake/SlicerCPack.cmake)
include(${Slicer_SOURCE_DIR}/CMake/SlicerExtensionGenerateConfig.cmake)
include(${Slicer_SOURCE_DIR}/CMake/SlicerExtensionCPack.cmake)

message("HDF5_ROOT: ${HDF5_ROOT}")
message("CPACK_INSTALL_CMAKE_PROJECTS: ${CPACK_INSTALL_CMAKE_PROJECTS}")