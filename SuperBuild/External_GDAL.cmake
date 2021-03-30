set(proj GDAL)

# Set dependency list
set(${proj}_DEPENDENCIES
  python
  python-pip
  python-setuptools
  curl
  )

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED GDAL_DIR AND NOT EXISTS ${GDAL_DIR})
  message(FATAL_ERROR "GDAL_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT Slicer_USE_SYSTEM_${proj})
  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})

  if(WIN32)
    set(GDAL_WHL "GDAL-3.1.4-cp36-cp36m-win_amd64.whl")
    set(GDAL_URL "https://download.lfd.uci.edu/pythonlibs/w4tscw6k/cp36/${GDAL_WHL}")

    ExternalProject_Add(${proj}
      ${${proj}_EP_ARGS}
      # URL ${GDAL_URL}
      DOWNLOAD_COMMAND curl -O ${GDAL_URL}
      DOWNLOAD_DIR ${EP_SOURCE_DIR}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ${PYTHON_EXECUTABLE} -m pip install ${EP_SOURCE_DIR}/${GDAL_WHL}
      DEPENDS
        ${${proj}_DEPENDENCIES}
      )

    ExternalProject_GenerateProjectDescription_Step(${proj})

  endif(WIN32)

  set(GDAL_ROOT "${Python3_ROOT_DIR}/Lib/site-packages/osgeo/")
  set(GDAL_DIR "${Python3_ROOT_DIR}/Lib/site-packages/osgeo/")
  set(GDAL_INCLUDE_DIR "${Python3_ROOT_DIR}/Lib/site-packages/osgeo/include/gdal")

else()
  # The project is provided using GDAL_DIR, nevertheless since other project may depend on GDAL,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    GDAL_INCLUDE_DIR:PATH
    GDAL_ROOT:PATH
    GDAL_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

ExternalProject_Message(${proj} "GDAL_INCLUDE_DIR:${GDAL_INCLUDE_DIR}")
ExternalProject_Message(${proj} "GDAL_ROOT:${GDAL_ROOT}")
ExternalProject_Message(${proj} "GDAL_DIR:${GDAL_DIR}")