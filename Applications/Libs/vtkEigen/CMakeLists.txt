project(vtkEigen)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.13.4)
#-----------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Options
# --------------------------------------------------------------------------
if(NOT DEFINED BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "Build with shared libraries." ON)
endif()

# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

#
# Eigen3_INCLUDE_DIR is defined in External_ITK.cmake
#

# # --------------------------------------------------------------------------
# # Include dirs
# # --------------------------------------------------------------------------
# set(include_dirs
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   ${CMAKE_CURRENT_BINARY_DIR}
#   ${Eigen3_INCLUDE_DIR}
#   )
# include_directories(${include_dirs})

# --------------------------------------------------------------------------
# Install headers
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  set(${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT ON)
endif()
if(NOT ${PROJECT_NAME}_INSTALL_NO_DEVELOPMENT)
  file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.(h|txx)")
  install(
    FILES ${headers} ${CMAKE_CURRENT_BINARY_DIR}/${configure_header_file}
    DESTINATION include/${PROJECT_NAME} COMPONENT Development)
endif()

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
set(vtkEigen_SRCS
  vtkEigenDoubleArray.cxx
  vtkEigenFloatArray.cxx
  vtkEigenProbeFilter.cxx
  vtkRaisedGrid.cxx
  )

# these types are never instantiated, so they don't
# get included in the vtkEigen lib file (hence they
# can't be wrapped for python)
set(vtkEigen_SRCS_not_used
  )

# Abstract/pure virtual classes

#set_source_files_properties(
#  ABSTRACT
#  )

# Helper classes

# set_source_files_properties(
#   WRAP_EXCLUDE
#   )

# --------------------------------------------------------------------------
# Build library
# --------------------------------------------------------------------------
set(lib_name ${PROJECT_NAME})

set(srcs ${vtkEigen_SRCS})
add_library(${lib_name} ${srcs})

set_target_properties(${lib_name} PROPERTIES LABELS ${lib_name})

# Apply user-defined properties to the library target.
if(Slicer_LIBRARY_PROPERTIES)
  set_target_properties(${lib_name} PROPERTIES ${Slicer_LIBRARY_PROPERTIES})
endif()

set(libs ${VTK_LIBRARIES})
target_link_libraries(${lib_name} ${libs})

# --------------------------------------------------------------------------
# Include dirs
# --------------------------------------------------------------------------
set(include_dirs
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Eigen3_INCLUDE_DIR}
  )
# target_include_directories(${lib_name} PUBLIC ${include_dirs})
include_directories(${include_dirs})

# --------------------------------------------------------------------------
# Folder
# --------------------------------------------------------------------------
if(NOT DEFINED ${PROJECT_NAME}_FOLDER)
  set(${PROJECT_NAME}_FOLDER ${PROJECT_NAME})
endif()
if(NOT "${${PROJECT_NAME}_FOLDER}" STREQUAL "")
  set_target_properties(${lib_name} PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
endif()

# --------------------------------------------------------------------------
# Export target
# --------------------------------------------------------------------------
# add ${lib_name} to SlicerTargets.cmake
set_property(GLOBAL APPEND PROPERTY Slicer_TARGETS ${lib_name})
# add ${lib_name}_INCLUDE_DIRS to SlicerConfig.cmake 
set_property(GLOBAL APPEND PROPERTY SLICER_MODULE_TARGETS ${lib_name})

if(NOT DEFINED ${PROJECT_NAME}_EXPORT_FILE)
  set(${PROJECT_NAME}_EXPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)
endif()
export(TARGETS ${lib_name} APPEND FILE ${${PROJECT_NAME}_EXPORT_FILE})

# --------------------------------------------------------------------------
# EXPORT DLL
# --------------------------------------------------------------------------
include(GenerateExportHeader)
generate_export_header(
  ${lib_name}
  EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_export.h
  )

# --------------------------------------------------------------------------
# Install library
# --------------------------------------------------------------------------
message("Slicer_LIB_DIR: ${Slicer_LIB_DIR}")
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_BIN_DIR)
  set(${PROJECT_NAME}_INSTALL_BIN_DIR bin)
endif()
if(NOT DEFINED ${PROJECT_NAME}_INSTALL_LIB_DIR)
  set(${PROJECT_NAME}_INSTALL_LIB_DIR ${Slicer_LIB_DIR})
endif()

install(TARGETS ${lib_name}
  RUNTIME DESTINATION ${${PROJECT_NAME}_INSTALL_BIN_DIR} COMPONENT RuntimeLibraries
  LIBRARY DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${${PROJECT_NAME}_INSTALL_LIB_DIR} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Python Wrapping
# --------------------------------------------------------------------------
if(VTK_WRAP_PYTHON)
  vtkMacroKitPythonWrap(
    KIT_NAME ${lib_name}
    KIT_SRCS ${vtkEigen_SRCS}
    KIT_INSTALL_BIN_DIR ${${PROJECT_NAME}_INSTALL_BIN_DIR}
    KIT_INSTALL_LIB_DIR ${${PROJECT_NAME}_INSTALL_LIB_DIR}
    )
  # Export target
  export(TARGETS ${lib_name}Python APPEND FILE ${${PROJECT_NAME}_EXPORT_FILE})
  if(${VTK_VERSION} VERSION_LESS "8.90")
    export(TARGETS ${lib_name}PythonD APPEND FILE ${${PROJECT_NAME}_EXPORT_FILE})
  endif()
  # Folder
  if(NOT "${${PROJECT_NAME}_FOLDER}" STREQUAL "")
    set_target_properties(${lib_name}Python PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
    if(${VTK_VERSION} VERSION_LESS "8.90")
      set_target_properties(${lib_name}PythonD PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
    endif()
    if(TARGET ${lib_name}Hierarchy)
      set_target_properties(${lib_name}Hierarchy PROPERTIES FOLDER ${${PROJECT_NAME}_FOLDER})
    endif()
  endif()
endif()

# --------------------------------------------------------------------------
# Testing (requires some of the examples)
# --------------------------------------------------------------------------
# if(BUILD_TESTING)
#   add_subdirectory(Testing)
# endif()

# --------------------------------------------------------------------------
# Set INCLUDE_DIRS variable
# --------------------------------------------------------------------------
set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "${PROJECT_NAME} include dirs" FORCE)
set(Slicer_Libs_INCLUDE_DIRS "${Slicer_Libs_INCLUDE_DIRS};${include_dirs}" CACHE INTERNAL "Slicer Libs includes" FORCE)

# --------------------------------------------------------------------------
# Configure support file
# --------------------------------------------------------------------------
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkEigenConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkEigenConfig.cmake
  )
