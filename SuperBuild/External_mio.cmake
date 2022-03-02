
set(proj mio)

# Set dependency list (ITK brings up Eigen lib)
set(${proj}_DEPENDENCIES "")

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED mio_ROOT AND NOT EXISTS ${mio_ROOT})
  message(FATAL_ERROR "mio_ROOT variable is defined but corresponds to nonexistent directory")
endif()

if((NOT DEFINED mio_ROOT
    OR NOT DEFINED mio_INCLUDE_DIR) AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/mandreyel/mio.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "3f86a95c0784d73ce6815237ec33ed25f233b643"
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

  set(mio_ROOT ${EP_SOURCE_DIR})
  set(mio_INCLUDE_DIR ${EP_SOURCE_DIR}/include)

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    mio_ROOT:PATH
    mio_INCLUDE_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "mio_ROOT: ${mio_ROOT}")
ExternalProject_Message(${proj} "mio_INCLUDE_DIR: ${mio_INCLUDE_DIR}")
