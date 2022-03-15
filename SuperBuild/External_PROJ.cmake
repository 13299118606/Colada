
set(proj PROJ)

# Set dependency list (ITK brings up Eigen lib)
set(${proj}_DEPENDENCIES python sqlite)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED PROJ_DIR AND NOT EXISTS ${PROJ_DIR})
  message(FATAL_ERROR "PROJ_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED PROJ_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/OSGeo/PROJ.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "9.0.0"
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
      -DENABLE_TIFF:BOOL=OFF 
      -DENABLE_CURL:BOOL=OFF 
      -DBUILD_PROJSYNC:BOOL=OFF 
      -DRUN_NETWORK_DEPENDENT_TESTS:BOOL=OFF 
      -DBUILD_TESTING:BOOL=OFF 
      -DSQLITE3_INCLUDE_DIR:PATH=${sqlite_INCLUDE_DIR}
      -DSQLITE3_LIBRARY:PATH=${sqlite_LIBRARY}
      -DEXE_SQLITE3:FILEPATH=${sqlite_EXE}
      -DPython_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(PROJ_DIR ${EP_INSTALL_DIR}/lib/cmake/proj)
  if(WIN32)
    list(APPEND GDAL_LIBS "${EP_INSTALL_DIR}/lib/proj.lib")
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/bin")
  else()
    list(APPEND GDAL_LIBS "${EP_INSTALL_DIR}/lib/libproj.so")
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/lib")
  endif()

  set(_lib_subdir lib)
  if(WIN32)
    set(_lib_subdir bin)
  endif()

  # library paths
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
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

  # pythonpath
  set(${proj}_PYTHONPATH_LAUNCHER_BUILD
    ${EP_INSTALL_DIR}/${_lib_subdir}
    )
  mark_as_superbuild(
    VARS ${proj}_PYTHONPATH_LAUNCHER_BUILD
    LABELS "PYTHONPATH_LAUNCHER_BUILD"
    )

  # envvars
  set(${proj}_ENVVARS_LAUNCHER_BUILD
    "PROJ_LIB=${EP_INSTALL_DIR}/share/proj"
    )
  mark_as_superbuild(
    VARS ${proj}_ENVVARS_LAUNCHER_BUILD
    LABELS "ENVVARS_LAUNCHER_BUILD"
    )

  set(${proj}_ENVVARS_LAUNCHER_INSTALLED
    "PROJ_LIB=<APPLAUNCHER_SETTINGS_DIR>/../share/proj"
    )
  mark_as_superbuild(
    VARS ${proj}_ENVVARS_LAUNCHER_INSTALLED
    LABELS "ENVVARS_LAUNCHER_INSTALLED"
    )

else()
  # The project is provided using PROJ_DIR, nevertheless since other project may depend on PROJ,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
  PROJ_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "PROJ_DIR: ${PROJ_DIR}")
