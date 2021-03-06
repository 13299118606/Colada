
set( proj sqlite )

# Set dependency list
set(${proj}_DEPENDENCIES "")

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  unset(${proj}_DIR CACHE)
  unset(${proj} CACHE)
  find_package(${proj} REQUIRED)
endif()

# Sanity checks
if(DEFINED ${proj}_DIR AND NOT EXISTS ${${proj}_DIR})
  message(FATAL_ERROR "${proj}_DIR variable is defined but corresponds to nonexistent directory")
endif()

if(NOT DEFINED ${proj}_DIR AND NOT Slicer_USE_SYSTEM_${proj})

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_REPOSITORY
    "${EP_GIT_PROTOCOL}://github.com/tierra-colada/sqlite-amalgamation.git"
    QUIET
    )

  ExternalProject_SetIfNotDefined(
    Slicer_${proj}_GIT_TAG
    "f2ada7ab2237696ed641f24fc2b07ad41ab83395"
    QUIET
    )

  set(EP_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})
  set(EP_BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(EP_INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-install)
  set(EP_INSTALL_LIBDIR "lib")

  set(${proj}_CMAKE_C_FLAGS ${ep_common_c_flags})
  if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-bit
    set(${proj}_CMAKE_C_FLAGS "${ep_common_c_flags} -fPIC")
  endif()

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    GIT_REPOSITORY "${Slicer_${proj}_GIT_REPOSITORY}"
    GIT_TAG "${Slicer_${proj}_GIT_TAG}"
    SOURCE_DIR ${EP_SOURCE_DIR}
    BINARY_DIR ${EP_BINARY_DIR}
    INSTALL_DIR ${EP_INSTALL_DIR}
    CMAKE_CACHE_ARGS
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_CXX_FLAGS:STRING=${ep_common_cxx_flags}
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_C_FLAGS:STRING=${${proj}_CMAKE_C_FLAGS}
      -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
      -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=${CMAKE_CXX_STANDARD_REQUIRED}
      -DCMAKE_CXX_EXTENSIONS:BOOL=${CMAKE_CXX_EXTENSIONS}
      -DCMAKE_INSTALL_LIBDIR:PATH=${EP_INSTALL_LIBDIR}
      -DSQLITE_BUILD_DOC:BOOL=OFF
      -DSQLITE_BUILD_EXAMPLES:BOOL=OFF
      -DSQLITE_BUILD_TESTS:BOOL=OFF
      # executable is needed by PROJ lib
      -DBUILD_SHELL:BOOL=ON
      # if PROJ is built as SHARED then it asks sqlite to be SHARED as well
      -DSQLITE_BUILD_SHARED:BOOL=ON 
      # GDAL recommends
      -DSQLITE_ENABLE_RTREE:BOOL=ON 
      -DSQLITE_ENABLE_MATH_FUNCTIONS:BOOL=ON 
      -DSQLITE_ENABLE_COLUMN_METADATA:BOOL=ON 
      -DSQLITE_OMIT_DECLTYPE:BOOL=OFF # must be OFF if SQLITE_ENABLE_COLUMN_METADATA is ON
      -DSQLITE_OMIT_DEPRECATED:BOOL=OFF # must be OFF or undefined external symbols on Windows
      -DSQLITE_OMIT_PROGRESS_CALLBACK:BOOL=OFF # must be OFF or undefined external symbols on Windows
      -DSQLITE_OMIT_SHARED_CACHE:BOOL=OFF # must be OFF or undefined external symbols on Windows
      # recommended options would define SQLITE_OMIT_DEPRECATED and SQLITE_OMIT_DECLTYPE,
      # which would cause build errors in Python, so go with default options instead
      -DBUILD_RECOMMENDED_OPTS:BOOL=ON # if python build fails then set it to OFF
      -DCMAKE_INSTALL_PREFIX:PATH=${EP_INSTALL_DIR}
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj})

  set(${proj}_DIR ${EP_BINARY_DIR})
  set(${proj}_INCLUDE_DIR ${EP_INSTALL_DIR}/include)
  if(WIN32)
    set(${proj}_LIBRARY ${EP_INSTALL_DIR}/lib/sqlite3.lib)
    set(${proj}_EXE ${EP_INSTALL_DIR}/bin/sqlite3.exe)
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/bin")
  else()
    set(${proj}_LIBRARY ${EP_INSTALL_DIR}/lib/libsqlite3.so)
    set(${proj}_EXE ${EP_INSTALL_DIR}/bin/sqlite3)
    list(APPEND GDAL_RUNTIME_DIRS "${EP_INSTALL_DIR}/lib")
  endif()
  list(APPEND GDAL_LIBS "${${proj}_LIBRARY}")

  if(WIN32)
    set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
      "${EP_INSTALL_DIR}/bin"
      )
  else()
    set(${proj}_LIBRARY_PATHS_LAUNCHER_BUILD 
      "${EP_INSTALL_DIR}/lib"
      )
  endif()
  mark_as_superbuild(
    VARS ${proj}_LIBRARY_PATHS_LAUNCHER_BUILD
    LABELS "LIBRARY_PATHS_LAUNCHER_BUILD"
    )

  # pythonpath
  set(${proj}_PYTHONPATH_LAUNCHER_BUILD
    ${EP_INSTALL_DIR}/lib
    )
  mark_as_superbuild(
    VARS ${proj}_PYTHONPATH_LAUNCHER_BUILD
    LABELS "PYTHONPATH_LAUNCHER_BUILD"
    )

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()

mark_as_superbuild(
  VARS
    ${proj}_DIR:PATH
    ${proj}_LIBRARY:PATH
    ${proj}_INCLUDE_DIR:PATH
    ${proj}_EXE:FILEPATH
  LABELS "FIND_PACKAGE"
  )
