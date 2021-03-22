
set(proj h5geo)

# Set dependency list
set(${proj}_DEPENDENCIES "tbb")

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
      ## CXX should not be needed, but it a cmake default test
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${ep_common_c_flags}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
      -DH5GEO_SUPERBUILD:BOOL=ON
      -DH5GEO_USE_THREADS:BOOL=ON
      -DH5GEO_BUILD_SHARED_LIBS:BOOL=ON
      -DH5GEO_BUILD_TESTS:BOOL=OFF
      -DH5GEO_USE_THREADS:BOOL=OFF
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(h5geo_DIR ${EP_INSTALL_DIR})
  set(h5geo_ROOT ${h5geo_DIR})
  set(h5geo_INCLUDE_DIR ${h5geo_DIR}/include)
  if(WIN32)
    set(h5geo_LIBRARY ${h5geo_DIR}/lib/h5geo.lib)
  else()
    set(h5geo_LIBRARY ${h5geo_DIR}/lib/libh5geo.a)
  endif()
else()
  # The project is provided using h5geo_DIR, nevertheless since other project may depend on h5geo,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    h5geo_INCLUDE_DIR:PATH
    h5geo_LIBRARY:FILEPATH
    h5geo_ROOT:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "h5geo_INCLUDE_DIR:${h5geo_INCLUDE_DIR}")
ExternalProject_Message(${proj} "h5geo_LIBRARY:${h5geo_LIBRARY}")
if(h5geo_ROOT)
  ExternalProject_Message(${proj} "h5geo_ROOT:${h5geo_ROOT}")
endif()
