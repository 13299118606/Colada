cmake_minimum_required(VERSION 3.13.4)

project(Seismic)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://www.slicer.org/wiki/Documentation/Nightly/Extensions/Seismic")
set(EXTENSION_CATEGORY "Seismic")
set(EXTENSION_CONTRIBUTORS "Kerim Khemraev (Homeless Corp.)")
set(EXTENSION_DESCRIPTION "Control prestack and stack seismic data")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/Seismic.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/Seismic/Screenshots/1.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})
#find_package(qColada REQUIRED)

find_package(OpenMP REQUIRED) # Colada needs OpenMP
find_package(ZLIB REQUIRED)
find_package(units REQUIRED CONFIG)  # based on units_DIR
find_package(HDF5 REQUIRED CONFIG)  # based on HDF5_DIR
find_package(h5geo REQUIRED CONFIG)  # based on h5geo_DIR
find_package(GDAL REQUIRED)

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(StackLoad)
add_subdirectory(Axes)
add_subdirectory(Zoom)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
