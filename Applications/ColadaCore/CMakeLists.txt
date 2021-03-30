project(ColadaCore)

include(SlicerMacroBuildBaseQtLibrary)

set(KIT_export_directive "COLADA_CORE_EXPORT")

find_package(Eigen3 3.3 REQUIRED CONFIG)
find_package(ZLIB REQUIRED)
find_package(HDF5 REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)
find_package(h5gt REQUIRED CONFIG)
find_package(h5geo REQUIRED CONFIG)
find_package(GDAL REQUIRED)

# Additional directories to include
set(KIT_include_directories
  )

# Source files
set(KIT_SRCS
  ColadaDBCore.cxx
  ColadaDBCore.h
  )

# Headers that should run through moc
set(KIT_MOC_SRCS
  )

# UI files
set(KIT_UI_SRCS
  )

# Additional Target libraries (Qt libs already included)
set(KIT_target_libraries
  ${QT_LIBRARIES} # Not needed: All dependencies are transitively satisfied by other targets
  Eigen3::Eigen
  magic_enum::magic_enum
  h5geo::hdf5-shared
  h5gt
  h5geo
  GDAL::GDAL
  )

# Resources
set(KIT_resources
  )

SlicerMacroBuildBaseQtLibrary(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  INCLUDE_DIRECTORIES ${KIT_include_directories}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_SRCS ${KIT_UI_SRCS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  # WRAP_PYTHONQT
  )

# --------------------------------------------------------------------------
# if(BUILD_TESTING)
#   add_subdirectory(Testing)
# endif()

