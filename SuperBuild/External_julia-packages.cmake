
set(proj julia-packages)

# Set dependency list
set(${proj}_DEPENDENCIES
  julia
  )

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

if(NOT Slicer_USE_SYSTEM_${proj})
  # needed for PyCall
  #set(ENV{PYTHON} ${PYTHON_EXECUTABLE})  # seems that doesn't work

  # set `PYTHON` env var
  configure_file(${CMAKE_SOURCE_DIR}/SuperBuild/julia-packages.jl.in 
    ${CMAKE_CURRENT_BINARY_DIR}/julia-packages.jl
    @ONLY
    )

  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND 
      ${julia_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/julia-packages.jl
    INSTALL_COMMAND 
      ${PYTHON_EXECUTABLE} -c "import os; os.environ['PATH'] = '${julia_DIR}/bin' + os.environ['PATH']; import julia; julia.install()"   # must be run after `julia-packages.jl` is done as it needs `PyCall` to be installed
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
