
#-----------------------------------------------------------------------------
set(MODULE_NAME StackLoad)
set(MODULE_TITLE "Stack Load")

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(Logic)
# All the MRML nodes are in StackLoad modoule.
# StackLoad module is required by all the others module.
# another solution can be to shift all the MRML to an AstroLibs folder
#add_subdirectory(MRMLDM)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Applications/ColadaApp
  ${Slicer_BINARY_DIR}/Applications/ColadaApp
  ${h5gt_INCLUDE_DIR}
  ${magic_enum_INCLUDE_DIR}
  ${Eigen3_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/MRMLDM
  ${CMAKE_CURRENT_BINARY_DIR}/MRMLDM
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  vtkEigenFloatArray.cxx
  vtkEigenFloatArray.h
  vtkEigenDoubleArray.cxx
  vtkEigenDoubleArray.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  qSlicer${MODULE_NAME}ModuleWidgets
#  vtkSlicer${MODULE_NAME}ModuleMRMLDisplayableManager
  vtkSlicerTransformsModuleLogic  # needed for my test disp manager
  qColadaApp
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildLoadableModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
#  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
#if(BUILD_TESTING)
#  add_subdirectory(Testing)
#endif()
