
set(Colada_PYTHON_SCRIPTS
  colada/__init__
  colada/qColadaDevReader
  colada/qColadaLasReader
  colada/qColadaMapReader
  )

set(Colada_PYTHON_MODULES_CONFIG "
# colada libs
'colada.qColadaDevReader',
'colada.qColadaLasReader',
'colada.qColadaMapReader',
"
  )

configure_file(
  colada/kits.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/colada/kits.py
  @ONLY
  )

list(APPEND Colada_PYTHON_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/colada/kits.py)

set(Colada_PYTHON_RESOURCES
  )

ctkMacroCompilePythonScript(
  TARGET_NAME ColadaBase
  SCRIPTS "${Colada_PYTHON_SCRIPTS}"
  RESOURCES "${Colada_PYTHON_RESOURCES}"
  DESTINATION_DIR ${Slicer_BINARY_DIR}/bin/Python
  INSTALL_DIR ${Slicer_INSTALL_BIN_DIR}
  )
