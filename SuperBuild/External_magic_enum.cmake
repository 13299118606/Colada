
set(proj magic_enum)

# Set dependency list
set(${proj}_DEPENDENCIES "")

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED magic_enum_DIR AND NOT EXISTS ${magic_enum_DIR})
  message(FATAL_ERROR "magic_enum_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED magic_enum_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/Neargye/magic_enum.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "v0.7.3"
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

  set(magic_enum_ROOT ${EP_SOURCE_DIR})
  set(magic_enum_DIR ${EP_SOURCE_DIR})
  set(magic_enum_INCLUDE_DIR ${EP_SOURCE_DIR}/include)
else()
  # The project is provided using magic_enum_DIR, nevertheless since other project may depend on magic_enum,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    magic_enum_ROOT:PATH
    magic_enum_DIR:PATH
    magic_enum_INCLUDE_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "magic_enum_ROOT: ${magic_enum_ROOT}")
ExternalProject_Message(${proj} "magic_enum_DIR: ${magic_enum_DIR}")
