
set(proj julia)

# Set dependency list
set(${proj}_DEPENDENCIES
  python
  python-pip
  python-setuptools
  python-packages
  )

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

# Sanity checks
if(DEFINED julia_DIR AND NOT EXISTS ${julia_DIR})
  message(FATAL_ERROR "julia_DIR variable is defined but corresponds to nonexistent directory")
endif()

set(julia_VER_MAJ 1)
set(julia_VER_MIN 6)
set(julia_VER_PATCH 1)

set(julia_VER 
  "${julia_VER_MAJ}.${julia_VER_MIN}.${julia_VER_PATCH}"  # MUST be qouted  "" to avoid semicolon delimiter
  )
  
set(julia_DIR 
  ${CMAKE_BINARY_DIR}/julia-${julia_VER_MAJ}.${julia_VER_MIN}
  )

# Variable expected by FindPython3 CMake module
set(julia_ROOT ${julia_DIR})
set(julia_INCLUDE_DIR ${julia_DIR}/include)
# `mark_as_superbuild` must not contain LIST (i.e. one file/dir per var)
if(WIN32)
  set(julia_LIB 
    "${julia_DIR}/lib/libjulia.dll.a"
    )
  set(julia_openlibm_LIB 
    "${julia_DIR}/lib/libopenlibm.dll.a"
    )
  set(julia_EXECUTABLE "${julia_DIR}/bin/julia.exe")
else()
  set(julia_LIB 
  "${julia_DIR}/lib/libjulia.so"
  )
  set(julia_EXECUTABLE "${julia_DIR}/bin/julia")
endif()

if(NOT EXISTS ${julia_EXECUTABLE} AND NOT Slicer_USE_SYSTEM_${proj})

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND 
      # --reinstall is needed as jill looks for julia in different folders
      ${PYTHON_EXECUTABLE} -m jill install ${julia_VER} --confirm --install_dir ${CMAKE_BINARY_DIR} --bypass_ssl --reinstall
      # ${PYTHON_EXECUTABLE} ${Colada_SOURCE_DIR}/SuperBuild/install_julia.py -v ${julia_VER} -p ${CMAKE_BINARY_DIR}
    LOG_INSTALL 1
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})
else()
  # The project is provided using julia_DIR, nevertheless since other project may depend on julia,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
  julia_ROOT:PATH
  julia_DIR:PATH
  julia_INCLUDE_DIR:PATH
  julia_LIB:FILEPATH
  julia_EXECUTABLE:FILEPATH
  LABELS "FIND_PACKAGE"
  )

if(WIN32)
mark_as_superbuild(
  VARS
  julia_openlibm_LIB:FILEPATH
  LABELS "FIND_PACKAGE"
  )
endif()

ExternalProject_Message(${proj} "julia_ROOT: ${julia_ROOT}")
ExternalProject_Message(${proj} "julia_INCLUDE_DIR: ${julia_INCLUDE_DIR}")
ExternalProject_Message(${proj} "julia_LIB: ${julia_LIB}")
ExternalProject_Message(${proj} "julia_EXECUTABLE: ${julia_EXECUTABLE}")

#-----------------------------------------------------------------------------
# Launcher setting specific to build tree

# library paths
if(WIN32)
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    ${julia_ROOT}/bin
    )
else()
  set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    ${julia_ROOT}/lib
    ${julia_ROOT}/lib/julia
    )
endif()

mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )


#-----------------------------------------------------------------------------
# Slicer Launcher setting specific to build tree
# library paths
set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
  ${julia_DIR}/lib
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )

# paths
set(${proj}_PATHS_LAUNCHER_BUILD 
  ${julia_DIR}/bin
  )
mark_as_superbuild(
  VARS ${proj}_PATHS_LAUNCHER_BUILD
  LABELS "PATHS_LAUNCHER_BUILD"
  )

#-----------------------------------------------------------------------------
# Slicer Launcher setting specific to install tree
# library paths
set(${proj}_LIBRARY_PATHS_LAUNCHER_INSTALLED 
  <APPLAUNCHER_SETTINGS_DIR>/../lib/julia/lib
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_INSTALLED
  LABELS "LIBRARY_PATHS_LAUNCHER_INSTALLED"
  )

# paths
set(${proj}_PATHS_LAUNCHER_INSTALLED 
  <APPLAUNCHER_SETTINGS_DIR>/../lib/julia/bin
  )

mark_as_superbuild(
  VARS ${proj}_PATHS_LAUNCHER_INSTALLED
  LABELS "PATHS_LAUNCHER_BUILD"
  )
