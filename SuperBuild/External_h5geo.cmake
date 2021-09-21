
set(proj h5geo)

# Set dependency list
set(${proj}_DEPENDENCIES tbb Eigen3 zlib HDF5 h5gt magic_enum python units)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED h5geo_DIR AND NOT EXISTS ${h5geo_DIR})
  message(FATAL_ERROR "h5geo_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED h5geo_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/TierraColada/h5geo.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "main"
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
      -DH5GEO_SUPERBUILD:BOOL=OFF
      -DH5GEO_USE_THREADS:BOOL=OFF
      -DH5GEO_BUILD_SHARED_LIBS:BOOL=ON
      -DH5GEO_BUILD_TESTS:BOOL=OFF
      -DH5GEO_USE_THREADS:BOOL=OFF
      -DH5GEO_BUILD_SHARED_LIBS:BOOL=ON
      -DH5GEO_BUILD_h5geopy:BOOL=ON
      -DHDF5_RUNTIME_DIR:PATH=${HDF5_RUNTIME_DIR}
      -DCOPY_H5GEOPY_RUNTIME_DEPS:BOOL=OFF
      -DHDF5_USE_STATIC_LIBRARIES:BOOL=OFF  
      # find package dirs
      -DTBB_DIR:PATH="${TBB_LIB_DIR}/../../../cmake"
      -DEIGEN3_INCLUDE_DIRS:PATH=${Eigen3_INCLUDE_DIR}
      -DZLIB_ROOT:PATH=${ZLIB_ROOT}
      -DHDF5_ROOT:PATH=${HDF5_ROOT}
      -Dh5gt_ROOT:PATH=${h5gt_ROOT}
      -DMAGIC_ENUM_INCLUDE_DIR:PATH=${magic_enum_INCLUDE_DIR}
      -Dunits_DIR:PATH=${units_DIR}
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(h5geo_ROOT ${EP_INSTALL_DIR})
  set(h5geo_DIR "${EP_INSTALL_DIR}/lib/cmake/h5geo")
else()
  # The project is provided using h5geo_DIR, nevertheless since other project may depend on h5geo,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    h5geo_ROOT:PATH
    h5geo_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "h5geo_ROOT: ${h5geo_ROOT}")
ExternalProject_Message(${proj} "h5geo_DIR: ${h5geo_DIR}")

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

# library paths
# if(UNIX)
#   set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
#     "${EP_BINARY_DIR}/lib/<CMAKE_CFG_INTDIR>"
#   )
# else()
#   set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
#     "${EP_BINARY_DIR}/<CMAKE_CFG_INTDIR>"
#   )
# endif()

set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
  "${EP_INSTALL_DIR}/lib"
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )