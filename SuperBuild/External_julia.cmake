
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
if(DEFINED julia_ROOT AND NOT EXISTS ${julia_ROOT})
  message(FATAL_ERROR "julia_ROOT variable is defined but corresponds to nonexistent directory")
endif()

set(julia_VER_MAJ 1)
set(julia_VER_MIN 6)
set(julia_VER_PATCH 5)

set(julia_VER 
  "${julia_VER_MAJ}.${julia_VER_MIN}.${julia_VER_PATCH}"  # MUST be qouted  "" to avoid semicolon delimiter
  )
  
set(julia_ROOT 
  ${CMAKE_BINARY_DIR}/julia-${julia_VER_MAJ}.${julia_VER_MIN}
  )

# Variable expected by FindPython3 CMake module
set(julia_INCLUDE_DIR ${julia_ROOT}/include)
# `mark_as_superbuild` must not contain LIST (i.e. one file/dir per var)
if(WIN32)
  set(julia_LIBRARY "${julia_ROOT}/lib/libjulia.dll.a")
  set(julia_openlibm_LIBRARY "${julia_ROOT}/lib/libopenlibm.dll.a")
  set(julia_EXECUTABLE "${julia_ROOT}/bin/julia.exe")
else()
  set(julia_LIBRARY "${julia_ROOT}/lib/libjulia.so")
  set(julia_EXECUTABLE "${julia_ROOT}/bin/julia")
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
  # The project is provided using julia_ROOT, nevertheless since other project may depend on julia,
  # let's add an 'empty' one
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    julia_ROOT:PATH
    julia_INCLUDE_DIR:PATH
    julia_LIBRARY:FILEPATH
    julia_EXECUTABLE:FILEPATH
  LABELS "FIND_PACKAGE"
  )

  if(WIN32)
    mark_as_superbuild(
      VARS
        julia_openlibm_LIBRARY:FILEPATH
      LABELS "FIND_PACKAGE"
      )
  endif()

ExternalProject_Message(${proj} "julia_ROOT: ${julia_ROOT}")
ExternalProject_Message(${proj} "julia_INCLUDE_DIR: ${julia_INCLUDE_DIR}")
ExternalProject_Message(${proj} "julia_LIBRARY: ${julia_LIBRARY}")
ExternalProject_Message(${proj} "julia_EXECUTABLE: ${julia_EXECUTABLE}")

#-----------------------------------------------------------------------------
# Slicer Launcher setting specific to build tree
# library paths
set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
  ${julia_ROOT}/lib
  )
mark_as_superbuild(
  VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
  LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
  )

# paths
set(${proj}_PATHS_LAUNCHER_BUILD 
  ${julia_ROOT}/bin
  )
mark_as_superbuild(
  VARS ${proj}_PATHS_LAUNCHER_BUILD
  LABELS "PATHS_LAUNCHER_BUILD"
  )
