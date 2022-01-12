
set(proj units)

# Set dependency list
set(${proj}_DEPENDENCIES "")

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED units_DIR AND NOT EXISTS ${units_DIR})
  message(FATAL_ERROR "units_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED units_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/LLNL/units"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "v0.5.0"
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
      -DUNITS_BUILD_CONVERTER_APP:BOOL=OFF
      -DUNITS_BUILD_FUZZ_TARGETS:BOOL=OFF
      -DUNITS_BUILD_SHARED_LIBRARY:BOOL=ON
      -DUNITS_BUILD_STATIC_LIBRARY:BOOL=OFF
      -DUNITS_ENABLE_TESTS:BOOL=OFF
      -DUNITS_HEADER_ONLY:BOOL=OFF
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(units_ROOT ${EP_INSTALL_DIR})
  set(units_DIR "${EP_INSTALL_DIR}/lib/cmake/units")
  set(units_INCLUDE_DIR ${EP_INSTALL_DIR}/include)
  if(WIN32)
    set(units_LIBRARY ${EP_INSTALL_DIR}/lib/units.lib)
  else()
    set(units_LIBRARY ${EP_INSTALL_DIR}/lib/libunits.so)
  endif()
else()
  # The project is provided using units_DIR, nevertheless since other project may depend on units,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    units_ROOT:PATH
    units_DIR:PATH
    units_INCLUDE_DIR:PATH
    units_LIBRARY:FILEPATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "units_ROOT: ${units_ROOT}")
ExternalProject_Message(${proj} "units_DIR: ${units_DIR}")
ExternalProject_Message(${proj} "units_INCLUDE_DIR: ${units_INCLUDE_DIR}")
ExternalProject_Message(${proj} "units_LIBRARY: ${units_LIBRARY}")

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

# library paths
if(WIN32)
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    ${units_ROOT}/bin
    )
else()
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    ${units_ROOT}/lib
    )
endif()

mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )
