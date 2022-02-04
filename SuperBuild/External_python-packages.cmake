set(proj python-packages)

message("External project: " ${proj})
message("Setting C++ standard - " ${CMAKE_CXX_STANDARD})

# Set dependency list
set(${proj}_DEPENDENCIES 
  python 
  python-numpy 
  python-scipy 
  python-pip
  )

if(NOT DEFINED Slicer_USE_SYSTEM_${proj})
  set(Slicer_USE_SYSTEM_${proj} ${Slicer_USE_SYSTEM_python})
endif()

# Include dependent projects if any
ExternalProject_Include_Dependencies(${proj} PROJECT_VAR proj DEPENDS_VAR ${proj}_DEPENDENCIES)

if(Slicer_USE_SYSTEM_${proj})
  message(FATAL_ERROR "Enabling Slicer_USE_SYSTEM_${proj} is not supported !")
endif()

if(NOT Slicer_USE_SYSTEM_${proj})
  # needed for PyCall
  set(ENV{PYTHON} ${PYTHON_EXECUTABLE})
  set(jill_URL https://github.com/tierra-colada/jill.py.git)

  # debugpy is installed as dep of ipykernel (installed along with devito)
  # and needed only for JupyterLab debugging.
  # As it causes errors during py packages compilation we should remove it
  ExternalProject_Add(${proj}
    ${${proj}_EP_ARGS}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND 
      ${PYTHON_EXECUTABLE} -m pip install -r ${Colada_SOURCE_DIR}/SuperBuild/python-packages.txt
    INSTALL_COMMAND 
      ${PYTHON_EXECUTABLE} -m pip uninstall -y debugpy
    COMMAND 
      ${PYTHON_EXECUTABLE} -m pip install git+${jill_URL}
    LOG_INSTALL 1
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  ExternalProject_GenerateProjectDescription_Step(${proj}
    VERSION ${_version}
    )

else()
  ExternalProject_Add_Empty(${proj} DEPENDS ${${proj}_DEPENDENCIES})
endif()
