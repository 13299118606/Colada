
set(proj Eigen3)

# Set dependency list
set(${proj}_DEPENDENCIES "")

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED Eigen3_DIR AND NOT EXISTS ${Eigen3_DIR})
  message(FATAL_ERROR "Eigen3_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED Eigen3_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "https://gitlab.com/libeigen/eigen.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "3.4-rc1"
    QUIET
    )

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${Slicer_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${Slicer_${proj}_GIT_TAG}"
    SOURCE_DIR ${EP_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS 
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(Eigen3_ROOT ${EP_SOURCE_DIR})
  set(Eigen3_DIR ${EP_SOURCE_DIR})
  set(Eigen3_INCLUDE_DIR ${Eigen3_ROOT})  # needed to find Eigen in h5geo
else()
  # The project is provided using Eigen3_DIR, nevertheless since other project may depend on Eigen3,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    Eigen3_ROOT:PATH
    Eigen3_DIR:PATH
    Eigen3_INCLUDE_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "Eigen3_ROOT: ${Eigen3_ROOT}")
ExternalProject_Message(${proj} "Eigen3_DIR: ${Eigen3_DIR}")
ExternalProject_Message(${proj} "Eigen3_INCLUDE_DIR: ${Eigen3_INCLUDE_DIR}")