#-----------------------------------------------------------------------------
set(MODULE_NAME GeoObjectInfo)

#-----------------------------------------------------------------------------
set(MODULE_PYTHON_SCRIPTS
  ${MODULE_NAME}.py
  )

set(MODULE_PYTHON_RESOURCES
  Resources/Icons/${MODULE_NAME}.png
  Resources/Icons/Gears.png
  Resources/UI/${MODULE_NAME}.ui
  Resources/UI/Settings.ui  
  Resources/GeoObjectInfo.qss  
  )

slicerFunctionAddPythonQtResources(MODULE_PYTHON_QRC_RESOURCES
  Resources/GeoObjectInfoResources.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildScriptedModule(
  NAME ${MODULE_NAME}
  SCRIPTS ${MODULE_PYTHON_SCRIPTS}  ${MODULE_PYTHON_QRC_RESOURCES}
  RESOURCES ${MODULE_PYTHON_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)

  # Register the unittest subclass in the main script as a ctest.
  # Note that the test will also be available at runtime.
  slicer_add_python_unittest(SCRIPT ${MODULE_NAME}.py)

  # Additional build-time testing
  add_subdirectory(Testing)
endif()
