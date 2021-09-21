message("julia_DIR: ${julia_DIR}")
message("Slicer_INSTALL_ROOT: ${Slicer_INSTALL_ROOT}")

install(
  DIRECTORY "${julia_DIR}"
  DESTINATION "${Slicer_INSTALL_ROOT}"
  )