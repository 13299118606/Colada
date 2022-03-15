
set(proj GDAL)

# Set dependency list (ITK brings up Eigen lib)
set(${proj}_DEPENDENCIES zlib HDF5 sqlite GEOS PROJ python-numpy)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED GDAL_DIR AND NOT EXISTS ${GDAL_DIR})
  message(FATAL_ERROR "GDAL_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED GDAL_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/OSGeo/gdal.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "68ceec0be4541da11cf9860141fb6aeae22f95e4"
    QUIET
    )

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${Slicer_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${Slicer_${proj}_GIT_TAG}"
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    INSTALL_DIR ${EP_INSTALL_DIR}
    CMAKE_CACHE_ARGS
      # CMake settings
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
      -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
      # Lib settings
      -DBUILD_TESTING:BOOL=OFF 
      -DZLIB_ROOT:PATH=${ZLIB_ROOT}
      -DPROJ_DIR:PATH=${PROJ_DIR}
      -DGEOS_DIR:PATH=${GEOS_DIR}
      -DHDF5_ROOT:PATH=${HDF5_ROOT}
      -DSQLite3_INCLUDE_DIR:PATH=${sqlite_INCLUDE_DIR}
      -DSQLite3_LIBRARY:PATH=${sqlite_LIBRARY}
      -DSQLite3_HAS_RTREE:BOOL=ON 
      -DSQLite3_HAS_COLUMN_METADATA:BOOL=ON 
      # GDAL uses CMAKE's findPython (Python_EXECUTABLE) while h5gt and h5geo use pybind11 (PYTHON_EXECUTABLE)
      -DPython_EXECUTABLE:PATH=${PYTHON_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )
  
  # copy site-packages from GDAL subdir to Slicer's python
  ExternalProject_Add(${proj}-copy-site-packages
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    INSTALL_DIR ${EP_INSTALL_DIR}
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    # https://cmake.org/cmake/help/latest/manual/cmake.1.html#command-line-tool-mode
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory 
      ${EP_INSTALL_DIR}/lib/python${Slicer_REQUIRED_PYTHON_VERSION_DOT}/site-packages
      ${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}
    DEPENDS ${proj}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(GDAL_ROOT ${EP_INSTALL_DIR})
  set(GDAL_DIR ${EP_INSTALL_DIR}/lib/cmake/gdal)
  if(WIN32)
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/bin")
  else()
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/lib")
  endif()

  set(_lib_subdir lib)
  if(WIN32)
    set(_lib_subdir bin)
  endif()

  # library paths (tbb lib added here because in External_tbb.cmake they are not added)
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
    ${TBB_LIB_DIR}
    ${EP_INSTALL_DIR}/${_lib_subdir}
    )
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )

  # paths
  set(${proj}_PATHS_LAUNCHER_BUILD ${EP_INSTALL_DIR}/bin)
  mark_as_superbuild(
    VARS ${proj}_PATHS_LAUNCHER_BUILD
    LABELS "PATHS_LAUNCHER_BUILD"
    )

  # envvars
  set(${proj}_ENVVARS_LAUNCHER_BUILD
    "GDAL_DATA=${EP_INSTALL_DIR}/share/gdal"
    )
  mark_as_superbuild(
    VARS ${proj}_ENVVARS_LAUNCHER_BUILD
    LABELS "ENVVARS_LAUNCHER_BUILD"
    )

  set(${proj}_ENVVARS_LAUNCHER_INSTALLED
    "GDAL_DATA=<APPLAUNCHER_SETTINGS_DIR>/../share/gdal"
    )
  mark_as_superbuild(
    VARS ${proj}_ENVVARS_LAUNCHER_INSTALLED
    LABELS "ENVVARS_LAUNCHER_INSTALLED"
    )

else()
  # The project is provided using GDAL_DIR, nevertheless since other project may depend on GDAL,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    GDAL_ROOT:PATH
    GDAL_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "GDAL_ROOT: ${GDAL_ROOT}")
ExternalProject_Message(${proj} "GDAL_DIR: ${GDAL_DIR}")
