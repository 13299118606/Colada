
set(proj h5gt)

# Set dependency list
set(${proj}_DEPENDENCIES Eigen3 zlib HDF5)

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED h5gt_DIR AND NOT EXISTS ${h5gt_DIR})
  message(FATAL_ERROR "h5gt_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED h5gt_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/TierraColada/h5gt.git"
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
      -DH5GT_USE_EIGEN:BOOL=ON
      -DH5GT_USE_XTENSOR:BOOL=OFF
      -DH5GT_USE_OPENCV:BOOL=OFF
      -DH5GT_BUILD_TESTS:BOOL=OFF
      -DH5GT_EXAMPLES:BOOL=OFF
      -DH5GT_PARALLEL_HDF5:BOOL=OFF
      -DH5GT_BUILD_h5gtpy:BOOL=OFF
      -DHDF5_USE_STATIC_LIBRARIES:BOOL=OFF
      # find package dirs
      -DEigen3_ROOT:PATH=${Eigen3_ROOT}
      -DZLIB_ROOT:PATH=${ZLIB_ROOT}
      -DHDF5_ROOT:PATH=${HDF5_ROOT}
    DEPENDS 
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(h5gt_ROOT ${EP_INSTALL_DIR})
  set(h5gt_DIR "${EP_INSTALL_DIR}/lib/cmake/h5gt")
  set(h5gt_INCLUDE_DIR "${EP_INSTALL_DIR}/include")
else()
  # The project is provided using h5gt_DIR, nevertheless since other project may depend on h5gt,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    h5gt_INCLUDE_DIR:PATH
    h5gt_ROOT:PATH
    h5gt_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "h5gt_INCLUDE_DIR:${h5gt_INCLUDE_DIR}")
ExternalProject_Message(${proj} "h5gt_ROOT:${h5gt_ROOT}")
ExternalProject_Message(${proj} "h5gt_DIR:${h5gt_DIR}")