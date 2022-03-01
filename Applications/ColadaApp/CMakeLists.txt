#============================================================================
#
# Copyright (c) Kitware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#============================================================================

project(ColadaApp)

include(SlicerMacroBuildApplication)

# --------------------------------------------------------------------------
# Properties
# --------------------------------------------------------------------------
SlicerReadApplicationProperties()

# --------------------------------------------------------------------------
# Folder
# --------------------------------------------------------------------------
set(${PROJECT_NAME}_FOLDER "App-${PROJECT_NAME}")

find_package(OpenMP REQUIRED) # Colada needs OpenMP
find_package(ZLIB REQUIRED)
find_package(units REQUIRED CONFIG)  # based on units_DIR
find_package(HDF5 REQUIRED CONFIG)  # based on HDF5_DIR
find_package(GDAL REQUIRED)

# GDAL::GDAL depends on many libs (TIFF, GEOS etc). Here we are linking them
if(DEFINED GDAL_LIBS)
  target_link_libraries(GDAL::GDAL INTERFACE ${GDAL_LIBS})
endif()

# Before finding 'h5geo' all its deps must be found (defined in 'h5geo-targets.cmake'):
# hdf5::hdf5-shared
# units::units
# GDAL::GDAL
# and tbb (but it is defined by library path)
# Linking to 'h5geo' is equal to link to listed above targets as well
find_package(h5geo REQUIRED CONFIG)  # based on h5geo_DIR

# --------------------------------------------------------------------------
# Application library
# --------------------------------------------------------------------------
set(APPLIB_NAME "q${PROJECT_NAME}")

set(APPLIB_SRCS
  qColadaAppMainWindow.cxx
  qColadaAppMainWindow.h

  util.cxx
  util.h
  
  qColadaStyle.cxx
  qColadaStyle.h
  qColadaDarkStyle.cxx
  qColadaDarkStyle.h
  qColadaLightStyle.cxx
  qColadaLightStyle.h
  qColadaApplication.cxx
  qColadaApplication.h
  qCRSDropLineEdit.h
  qCRSDropLineEdit.cxx
  qCRSDropTableView.h
  qCRSDropTableView.cxx
  qCRSWidget.cxx
  qCRSWidget.h
  qREValidator.cxx
  qREValidator.h
  qCheckableHHeaderView.cxx
  qCheckableHHeaderView.h
  qCheckableVHeaderView.cxx
  qCheckableVHeaderView.h
  qColadaH5TreeView.cxx
  qColadaH5TreeView.h
  qColadaH5Item.cxx
  qColadaH5Item.h
  qColadaH5ItemDelegate.cxx
  qColadaH5ItemDelegate.h
  qColadaH5Model.cxx
  qColadaH5Model.h
  qColadaH5ProxyModel.cxx
  qColadaH5ProxyModel.h
  qColadaH5FileDialog.h
  qColadaH5FileDialog.cxx
  qColadaReader.h
  qColadaReader.cxx
  qColadaSegyReader.h
  qColadaSegyReader.cxx

  qColadaGainWidget.cxx
  qColadaGainWidget.h

  SegyRead.h
  SegyRead.cxx

  qH5ContainerTypeComboBox.h
  qH5ContainerTypeComboBox.cxx
  qH5ObjectTypeComboBox.h
  qH5ObjectTypeComboBox.cxx
  qH5SurveyTypeComboBox.h
  qH5SurveyTypeComboBox.cxx
  qH5SeisDataTypeComboBox.h
  qH5SeisDataTypeComboBox.cxx
  qH5DomainComboBox.h
  qH5DomainComboBox.cxx
  qH5CreateFlagComboBox.h
  qH5CreateFlagComboBox.cxx
  qH5ItemDropLineEdit.h
  qH5ItemDropLineEdit.cxx
  qH5SeisSortWidget.h
  qH5SeisSortWidget.cxx
  qCopyPasteTableView.h
  qCopyPasteTableView.cxx
  qComboBoxDelegate.h
  qComboBoxDelegate.cxx
  qScienceSpinBox.h
  qScienceSpinBox.cxx
  qSpinBoxDelegate.h
  qSpinBoxDelegate.cxx
  qCompletableComboBox.h
  qCompletableComboBox.cxx
  qScienceSpinBoxDelegate.h
  qScienceSpinBoxDelegate.cxx
  qLineEditDelegate.h
  qLineEditDelegate.cxx
  qPathEditDelegate.h
  qPathEditDelegate.cxx
  qSharedListViews.h
  qSharedListViews.cxx
  qSharedTableViews.h
  qSharedTableViews.cxx

  qMRMLCompletableNodeComboBox.h
  qMRMLCompletableNodeComboBox.cxx

  qColadaAppPythonQtDecorators.h
  )

set(APPLIB_MOC_SRCS
  qColadaAppMainWindow.h
  qColadaStyle.h
  qColadaDarkStyle.h
  qColadaLightStyle.h

  qColadaApplication.h
  qCRSDropLineEdit.h
  qCRSDropTableView.h
  qCRSWidget.h
  qREValidator.h
  qCheckableHHeaderView.h
  qCheckableVHeaderView.h
  qColadaH5TreeView.h
  qColadaH5ItemDelegate.h
  qColadaH5Model.h
  qColadaH5ProxyModel.h
  qColadaH5FileDialog.h
  qColadaReader.h
  qColadaSegyReader.h

  qColadaGainWidget.h

  qH5ContainerTypeComboBox.h
  qH5ObjectTypeComboBox.h
  qH5SurveyTypeComboBox.h
  qH5SeisDataTypeComboBox.h
  qH5DomainComboBox.h
  qH5CreateFlagComboBox.h
  qH5ItemDropLineEdit.h
  qH5SeisSortWidget.h
  qCopyPasteTableView.h
  qScienceSpinBox.h
  qComboBoxDelegate.h
  qSpinBoxDelegate.h
  qCompletableComboBox.h
  qScienceSpinBoxDelegate.h
  qLineEditDelegate.h
  qPathEditDelegate.h
  qSharedListViews.h
  qSharedTableViews.h

  qMRMLCompletableNodeComboBox.h

  qColadaAppPythonQtDecorators.h
  )

set(APPLIB_UI_SRCS
  )

set(APPLIB_RESOURCES
  Resources/App.qrc
  )

set(APPLIB_TARGET_LIBRARIES
  # ColadaCore  # this may be useful when splitting qColadaApp by few subprojects
  OpenMP::OpenMP_CXX
  h5geo
  ${julia_LIBRARY}
  )

if(WIN32)
  list(APPEND APPLIB_TARGET_LIBRARIES ${julia_openlibm_LIBRARY})
endif()

set(APPLIB_DIRS 
  ${Eigen3_INCLUDE_DIR}
  ${h5gt_INCLUDE_DIR}
  ${magic_enum_INCLUDE_DIR}
  ${mio_INCLUDE_DIR}
  ${julia_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

foreach(var ${APPLIB_DIRS})
  if(NOT EXISTS "${var}")
    message(FATAL_ERROR "Path to include dir: ${var} doesn't exist!")
  endif()
endforeach()

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

# add ${APPLIB_NAME}_INCLUDE_DIRS to SlicerConfig.cmake 
set_property(GLOBAL APPEND PROPERTY SLICER_MODULE_TARGETS ${APPLIB_NAME})

# --------------------------------------------------------------------------
# Plugins that depend on qColadaApp
# --------------------------------------------------------------------------

add_subdirectory(StylePlugins)

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

# --------------------------------------------------------------------------
# Install extension deps packages
# --------------------------------------------------------------------------

install(
  DIRECTORY "${julia_ROOT}/"                  # '/' in the end of path is necessary
  DESTINATION ${Slicer_INSTALL_ROOT}
  COMPONENT Runtime
  PATTERN "LICENSE.md" EXCLUDE
  )

if(WIN32)
  install(
    DIRECTORY "${HDF5_ROOT}/bin/"
    DESTINATION ${Slicer_INSTALL_BIN_DIR}
    COMPONENT Runtime
    )
  install(
    DIRECTORY "${h5geo_ROOT}/lib/"
    DESTINATION ${Slicer_INSTALL_BIN_DIR}
    COMPONENT Runtime
    PATTERN "cmake" EXCLUDE
    PATTERN "*.lib" EXCLUDE
    )
  install(
    DIRECTORY "${units_ROOT}/lib/"
    DESTINATION ${Slicer_INSTALL_BIN_DIR}
    COMPONENT Runtime
    PATTERN "cmake" EXCLUDE
    PATTERN "*.lib" EXCLUDE
  )
else()
  # HDF5 tools are in 'bin' folder
  install(
    DIRECTORY "${HDF5_ROOT}/bin/"
    DESTINATION ${Slicer_INSTALL_BIN_DIR}
    COMPONENT Runtime
    )
  install(
    DIRECTORY "${HDF5_ROOT}/lib/"
    DESTINATION ${Slicer_INSTALL_LIB_DIR}
    COMPONENT Runtime
    PATTERN "pkgconfig" EXCLUDE
    PATTERN "*.a" EXCLUDE                     # exclude static lib
    )
  install(
    DIRECTORY "${h5geo_ROOT}/lib/"
    DESTINATION ${Slicer_INSTALL_LIB_DIR}
    COMPONENT Runtime
    PATTERN "cmake" EXCLUDE
    )
  install(
    DIRECTORY "${units_ROOT}/lib/"
    DESTINATION ${Slicer_INSTALL_LIB_DIR}
    COMPONENT Runtime
    PATTERN "cmake" EXCLUDE
    )
endif()

# install(
#   FILES ${HDF5_ROOT}/lib/hdf5
#   DESTINATION ${Slicer_INSTALL_LIB_DIR}
#   COMPONENT Runtime 
#   )
# install(
#   FILES ${h5geo_ROOT}/lib/h5geo
#   DESTINATION ${Slicer_INSTALL_LIB_DIR}
#   COMPONENT Runtime 
#   )

# # Install libraries
# include(${Slicer_SOURCE_DIR}/CMake/SlicerFunctionInstallLibrary.cmake)
# slicerInstallLibrary(
#   FILE ${HDF5_ROOT}/lib/hdf5
#   DESTINATION ${Slicer_INSTALL_BIN_DIR}
#   COMPONENT RuntimeLibraries
#   )
# slicerInstallLibrary(
#   FILE ${h5geo_ROOT}/lib/h5geo
#   DESTINATION ${Slicer_INSTALL_BIN_DIR}
#   COMPONENT RuntimeLibraries
#   )

# set(EXTENSION_NAME Colada)
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${HDF5_ROOT};HDF5;ALL;/")
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${h5geo_ROOT};h5geo;ALL;/")
# set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${julia_ROOT};julia;ALL;/")

#-----------------------------------------------------------------------------
# list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
# list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
# include(${Slicer_EXTENSION_GENERATE_CONFIG})
# include(${Slicer_SOURCE_DIR}/CMake/SlicerCPack.cmake)

# include(${Slicer_SOURCE_DIR}/CMake/SlicerExtensionGenerateConfig.cmake)
# include(${Slicer_SOURCE_DIR}/CMake/SlicerExtensionCPack.cmake)

# message("HDF5_ROOT: ${HDF5_ROOT}")
# message("CPACK_INSTALL_CMAKE_PROJECTS: ${CPACK_INSTALL_CMAKE_PROJECTS}")

# --------------------------------------------------------------------------
# Set INCLUDE_DIRS variable to SlicerConfig.cmake
# --------------------------------------------------------------------------
set(${APPLIB_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "${APPLIB_NAME} include dirs" FORCE)
