
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
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)

  if(WIN32)
    set(GDAL_URL "https://download.lfd.uci.edu/pythonlibs/x6hvwk7i/GDAL-3.4.1-cp39-cp39-win_amd64.whl")  # https://www.lfd.uci.edu/~gohlke/pythonlibs/#gdal
  elseif(UNIX)
    set(GDAL_URL "https://master.dl.sourceforge.net/project/gdal-wheels-for-linux/GDAL-3.4.1-cp39-cp39-manylinux_2_5_x86_64.manylinux1_x86_64.whl?viasf=1")  # https://sourceforge.net/projects/gdal-wheels-for-linux/files/
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DOWNLOAD_COMMAND ""
    DOWNLOAD_DIR ${EP_SOURCE_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${PYTHON_EXECUTABLE} -m pip install ${GDAL_URL}
    DEPENDS
      ${${proj}_DEPENDENCIES}
  )

  ExternalProject_GenerateProjectDescription_Step(${proj})
  
  set(GDAL_ROOT "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo")
  set(GDAL_DIR "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo")
  set(GDAL_INCLUDE_DIR "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/include") # needed by FindGDAL
  if(WIN32)
    set(GDAL_LIBRARY "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/lib/gdal_i.lib")
  else()
    set(GDAL_LIBRARY "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/lib/libgdal.so")
    set(GDAL_LIBS_DIR "${python_DIR}/${PYTHON_SITE_PACKAGES_SUBDIR}/GDAL.libs")
    file(GLOB GDAL_LIBS
      "${GDAL_LIBS_DIR}/lib*"
    )
  endif()

  mark_as_superbuild(
  VARS
    GDAL_INCLUDE_DIR:PATH
    GDAL_LIBRARY:FILEPATH  
    GDAL_ROOT:PATH
    GDAL_DIR:PATH
    GDAL_LIBS_DIR:PATH
  LABELS "FIND_PACKAGE"
  )

else()
  # The project is provided using GDAL_DIR, nevertheless since other project may depend on GDAL,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

ExternalProject_Message(${proj} "GDAL_INCLUDE_DIR: ${GDAL_INCLUDE_DIR}")
ExternalProject_Message(${proj} "GDAL_LIBRARY: ${GDAL_LIBRARY}")
ExternalProject_Message(${proj} "GDAL_ROOT: ${GDAL_ROOT}")
ExternalProject_Message(${proj} "GDAL_DIR: ${GDAL_DIR}")
ExternalProject_Message(${proj} "GDAL_LIBS_DIR: ${GDAL_LIBS_DIR}")

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

# library paths
set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  ${GDAL_ROOT}
  ${GDAL_ROOT}/data/proj
  ${GDAL_LIBS_DIR}
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )

set(${proj}_ENVVARS_LAUNCHER_BUILD
  "PROJ_LIB=${GDAL_ROOT}/data/proj"
  )
mark_as_superbuild(
  VARS ${proj}_ENVVARS_LAUNCHER_BUILD
  LABELS "ENVVARS_LAUNCHER_BUILD"
  )

# pythonpath (add GDAL libs dir to PythonSlicer launcher build settings)
set(${proj}_PYTHONPATH_LAUNCHER_BUILD
  ${GDAL_LIBS_DIR}
  )
mark_as_superbuild(
  VARS ${proj}_PYTHONPATH_LAUNCHER_BUILD
  LABELS "PYTHONPATH_LAUNCHER_BUILD"
  )


#-----------------------------------------------------------------------------
# Launcher setting specific to install tree

# library paths
# Windows and Linux has different `site-packages` subdir
set(${proj}_LIBRARY_PATHS_LAUNCHER_INSTALLED 
  <APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo
  <APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/lib
  <APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/data/proj
  <APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/GDAL.libs
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_INSTALLED
  LABELS "LIBRARY_PATHS_LAUNCHER_INSTALLED"
  )

set(${proj}_ENVVARS_LAUNCHER_INSTALLED 
  "PROJ_LIB=<APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/osgeo/data/proj"
  )

mark_as_superbuild(
  VARS ${proj}_ENVVARS_LAUNCHER_INSTALLED 
  LABELS "ENVVARS_LAUNCHER_INSTALLED"
  )

# pythonpath (add GDAL libs dir to PythonSlicer launcher install settings)
set(${proj}_PYTHONPATH_LAUNCHER_INSTALLED
  <APPLAUNCHER_SETTINGS_DIR>/../lib/Python/${PYTHON_SITE_PACKAGES_SUBDIR}/GDAL.libs
  )
mark_as_superbuild(
  VARS ${proj}_PYTHONPATH_LAUNCHER_INSTALLED
  LABELS "PYTHONPATH_LAUNCHER_INSTALLED"
  )