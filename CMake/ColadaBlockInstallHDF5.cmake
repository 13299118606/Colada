message("HDF5_DIR: ${HDF5_DIR}")
message("Slicer_INSTALL_ROOT: ${Slicer_INSTALL_ROOT}")

install(
  DIRECTORY "${HDF5_DIR}"
  DESTINATION "${Slicer_INSTALL_ROOT}"
  )