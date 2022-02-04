cmake_minimum_required(VERSION 3.13.4)

# Enable C++17
if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

# Slicer sources
include(FetchContent)
if(NOT DEFINED slicersources_SOURCE_DIR)
  # Download Slicer sources and set variables slicersources_SOURCE_DIR and slicersources_BINARY_DIR
  FetchContent_Populate(slicersources
    GIT_REPOSITORY https://github.com/jamesobutler/Slicer	# git://github.com/Slicer/Slicer
    GIT_TAG        "1c1e7faa5c4b515aa160d2d1df5b9befb5e48b72"   # "c72d890515ccca70e4173dfcedd370e197c44ae9"
    GIT_PROGRESS   1
    )
else()
  set(slicersources_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/slicersources-subbuild)
endif()

# copy external HDF5, VTK, ITK, CTK to Slicer sources
# configure_file(
#   "${CMAKE_CURRENT_SOURCE_DIR}/CMake/External_HDF5.cmake" 
#   "${slicersources_SOURCE_DIR}/SuperBuild/External_HDF5.cmake" COPYONLY
#   )
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/External_VTK.cmake" 
    "${slicersources_SOURCE_DIR}/SuperBuild/External_VTK.cmake" COPYONLY
  )
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/External_ITK.cmake" 
  "${slicersources_SOURCE_DIR}/SuperBuild/External_ITK.cmake" COPYONLY
  )
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/External_CTK.cmake" 
  "${slicersources_SOURCE_DIR}/SuperBuild/External_CTK.cmake" COPYONLY
  )
#configure_file(
#  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/SuperBuild.cmake" 
#  "${slicersources_SOURCE_DIR}/SuperBuild.cmake" COPYONLY
#  )

# macOS initialization
set(CMAKE_MODULE_PATH ${slicersources_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})
include(SlicerInitializeOSXVariables)

project(Colada)

# Configure Application
set(Slicer_APPLICATIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Applications)
set(Slicer_MAIN_PROJECT "ColadaApp")

# Set organization
set(Slicer_ORGANIZATION_DOMAIN "kitware.com")
set(Slicer_ORGANIZATION_NAME   "Kitware, Inc.")

# Default home and favorite modules
set(Slicer_DEFAULT_HOME_MODULE "Home")
set(Slicer_DEFAULT_FAVORITE_MODULES "Data, Volumes, Models, Transforms, Markups, SegmentEditor")

# Configure SuperBuild
set(SUPERBUILD_TOPLEVEL_PROJECT Slicer)
set(EXTERNAL_PROJECT_ADDITIONAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild")
include(ExternalProjectDependency)

# Additional Slicer dependencies looked up in EXTERNAL_PROJECT_ADDITIONAL_DIR
set(Slicer_ADDITIONAL_DEPENDENCIES
  GDAL
  h5geo
  h5gt
  HDF5
  magic_enum
  python-packages
  julia
  julia-packages
  )

#  Enable listed remote modules from ITK 
set(Slicer_ITK_ADDITIONAL_MODULES
  )

if(NOT CMAKE_CONFIGURATION_TYPES)
  set(Slicer_DEFAULT_BUILD_TYPE "Release")
endif()
include(SlicerInitializeBuildType)
include(SlicerInitializeReleaseType)

# Set application bundle identifier for macOS
if(APPLE)
  set(Slicer_MACOSX_BUNDLE_GUI_IDENTIFIER "com.kitware.colada")
endif()

# Installation folder and admin account requirement for Windows
if(WIN32)
  # Note: To avoid escaping issue, make sure to use forward slash when setting
  #       "Slicer_CPACK_NSIS_INSTALL_ROOT". It is replaced by "\\\\" in SlicerCPack.
  set(Slicer_CPACK_NSIS_INSTALL_REQUIRES_ADMIN_ACCOUNT ON)
  if(Slicer_CPACK_NSIS_INSTALL_REQUIRES_ADMIN_ACCOUNT)
    # User has administrative privileges, therefore we can install to shared folder
    # "C:\Program Files" or "C:\Program Files (x86)".
    if(CMAKE_CL_64)
      set(Slicer_CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    else()
      set(Slicer_CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    endif()
  else()
    # We do not require administrative privileges, therefore we install to user folder
    # "C:\Users\<username>\AppData\Local".
    set(Slicer_CPACK_NSIS_INSTALL_ROOT "$LOCALAPPDATA/${Slicer_ORGANIZATION_NAME}")
  endif()
endif()

# Slicer options
option(BUILD_TESTING                            "Build application test suite"                        ON)
option(Slicer_BUILD_DOCUMENTATION               "Build documentation (Doxygen, sphinx, ...)"          OFF)
if(WIN32)
  option(Slicer_BUILD_WIN32_CONSOLE             "Build application executable as a console app"       OFF)
endif()

# option(Slicer_BUILD_DICOM_SUPPORT               "Build application with DICOM support"                OFF)
# option(Slicer_BUILD_DIFFUSION_SUPPORT           "Build application with Diffusion support"            OFF)
# option(Slicer_BUILD_EXTENSIONMANAGER_SUPPORT    "Build application with ExtensionManager support"     ON)
# option(Slicer_BUILD_MULTIVOLUME_SUPPORT         "Build application with MultiVolume support"          OFF)
# option(Slicer_BUILD_PARAMETERSERIALIZER_SUPPORT "Build application with parameter serializer support" OFF)
# option(Slicer_USE_PYTHONQT                      "Build application with Python support"               ON)
# option(Slicer_USE_QtTesting                     "Build application with QtTesting support"            ON)
# option(Slicer_USE_SimpleITK                     "Build application with SimpleITK support"            OFF)

# option(Slicer_BUILD_BRAINSTOOLS                 "Build application with BRAINSTools module"           OFF)
# option(Slicer_BUILD_DataStore                   "Build application with DataStore module"             OFF)
# option(Slicer_BUILD_CompareVolumes              "Build application with ChangeTrackerPy module"       OFF)
# option(Slicer_BUILD_LandmarkRegistration        "Build application with LandmarkRegistration module"  OFF)
# option(Slicer_BUILD_SurfaceToolbox              "Build application with SurfaceToolbox module"        ON)


# override option values as option makes sense only when these vars 
# are not in the cache (i.e. when cmake runs for a first time in this case)

set(Slicer_BUILD_DICOM_SUPPORT                ON CACHE BOOL    "Build application with DICOM support"                FORCE)
set(Slicer_BUILD_DIFFUSION_SUPPORT            OFF CACHE BOOL    "Build application with Diffusion support"            FORCE)
set(Slicer_BUILD_EXTENSIONMANAGER_SUPPORT     ON CACHE BOOL    "Build application with ExtensionManager support"      FORCE)
set(Slicer_BUILD_MULTIVOLUME_SUPPORT          ON CACHE BOOL    "Build application with MultiVolume support"          FORCE)  # requires DCMTK (DICOM)
set(Slicer_BUILD_PARAMETERSERIALIZER_SUPPORT  OFF CACHE BOOL    "Build application with parameter serializer support" FORCE)
set(Slicer_USE_PYTHONQT                       ON CACHE BOOL    "Build application with Python support"                FORCE)
set(Slicer_USE_QtTesting                      ON CACHE BOOL    "Build application with QtTesting support"             FORCE)
set(Slicer_USE_SimpleITK                      OFF CACHE BOOL    "Build application with SimpleITK support"            FORCE)

set(Slicer_BUILD_BRAINSTOOLS                  OFF CACHE BOOL    "Build application with BRAINSTools module"           FORCE)
set(Slicer_BUILD_DataStore                    ON CACHE BOOL    "Build application with DataStore module"             FORCE)
set(Slicer_BUILD_CompareVolumes               ON CACHE BOOL    "Build application with ChangeTrackerPy module"       FORCE)
set(Slicer_BUILD_LandmarkRegistration         OFF CACHE BOOL    "Build application with LandmarkRegistration module"  FORCE)
set(Slicer_BUILD_SurfaceToolbox               ON CACHE BOOL    "Build application with SurfaceToolbox module"         FORCE)

message("BUILD_TESTING: ${BUILD_TESTING}")
message("Slicer_BUILD_DOCUMENTATION: ${Slicer_BUILD_DOCUMENTATION}")
message("Slicer_BUILD_WIN32_CONSOLE: ${Slicer_BUILD_WIN32_CONSOLE}")
message("Slicer_BUILD_DICOM_SUPPORT: ${Slicer_BUILD_DICOM_SUPPORT}")
message("Slicer_BUILD_DIFFUSION_SUPPORT: ${Slicer_BUILD_DIFFUSION_SUPPORT}")
message("Slicer_BUILD_EXTENSIONMANAGER_SUPPORT: ${Slicer_BUILD_EXTENSIONMANAGER_SUPPORT}")
message("Slicer_BUILD_MULTIVOLUME_SUPPORT: ${Slicer_BUILD_MULTIVOLUME_SUPPORT}")
message("Slicer_BUILD_PARAMETERSERIALIZER_SUPPORT: ${Slicer_BUILD_PARAMETERSERIALIZER_SUPPORT}")
message("Slicer_USE_PYTHONQT: ${Slicer_USE_PYTHONQT}")
message("Slicer_USE_QtTesting: ${Slicer_USE_QtTesting}")
message("Slicer_USE_SimpleITK: ${Slicer_USE_SimpleITK}")

message("Slicer_BUILD_BRAINSTOOLS: ${Slicer_BUILD_BRAINSTOOLS}")
message("Slicer_BUILD_DataStore: ${Slicer_BUILD_DataStore}")
message("Slicer_BUILD_CompareVolumes: ${Slicer_BUILD_CompareVolumes}")
message("Slicer_BUILD_LandmarkRegistration: ${Slicer_BUILD_LandmarkRegistration}")
message("Slicer_BUILD_SurfaceToolbox: ${Slicer_BUILD_SurfaceToolbox}")

# Force Slicer to use VTK 9 (this must be CACHE var with FORCE parameter)
set(Slicer_VTK_VERSION_MAJOR 9 CACHE STRING "VTK major version (8 or 9)" FORCE)

# Force Slicer to build with TBB support
set(Slicer_VTK_SMP_IMPLEMENTATION_TYPE TBB  CACHE STRING "Which multi-threaded parallelism implementation to use in VTK. Options are Sequential or TBB." FORCE)

# Enable Slicer built-in modules
set(Slicer_CLIMODULES_ENABLED
  ResampleDTIVolume             # Needed by ResampleScalarVectorDWIVolume
  ResampleScalarVectorDWIVolume # Depends on DiffusionApplications, needed by CropVolume
  )
set(Slicer_QTLOADABLEMODULES_ENABLED
  Annotations       
  Cameras           
  Colors            
  CropVolume        
  Data              
  DoubleArrays      
  Markups           
  Models            
  Plots             
  Reformat          
  SceneViews        
  Segmentations     
  Sequences         
  SlicerWelcome     
  SubjectHierarchy  
  Tables            
  Terminologies     
  Texts             
  Transforms        
  Units             
  ViewControllers   
  VolumeRendering   
  Volumes           
#
#
#  Cameras
#  Terminologies
#  Volumes
#  Colors                      # Try to explicitely enable to avoid errors
#  Data                        # Try to explicitely enable to avoid errors
#  Annotations                 # Try to explicitely enable to avoid errors
#  Markups                     # Try to explicitely enable to avoid errors
#  SubjectHierarchy            # Try to explicitely enable to avoid errors
#  SceneViews                  # Gives error: unknown #include <vtkObject.h>
#  ViewControllers
  )
set(Slicer_QTSCRIPTEDMODULES_ENABLED
  CropVolumeSequence  
  DataProbe           
  DICOM               
  DICOMLib            
  DICOMPatcher        
  DICOMPlugins        
  DMRIInstall         
  Editor              
  EditorLib           
  Endoscopy           
  ExtensionWizard     
  LabelStatistics     
  PerformanceTests    
  SampleData          
  ScreenCapture       
  SegmentEditor       
  SegmentStatistics   
  SelfTests           
  VectorToScalarVolume

  # DataProbe                     # Manually added modules (I need it)
  # SampleData                    # Manually added modules (I need it)
  # see other modules that can be added 
  # from slicersources-src folder for example:
  # LabelStatistics
  # PerformanceTests
  # VectorToScalarVolume
  )

# Disable Slicer built-in modules
set(Slicer_CLIMODULES_DISABLED
  )
set(Slicer_QTLOADABLEMODULES_DISABLED
  #SceneViews                  # was disabled by default (but I need it)
  SlicerWelcome
  #ViewControllers             # was disabled by default (but I need it)
  )
set(Slicer_QTSCRIPTEDMODULES_DISABLED
  # DataProbe                   # was disabled by default (but I need it)
  # DMRIInstall
  # Endoscopy
  # LabelStatistics
  # PerformanceTests
  # SampleData                  # was disabled by default (easier to test app)
  # VectorToScalarVolume
  )

# Enable/Disable Slicer custom modules: To create a new module, use the SlicerExtensionWizard.
set(Slicer_EXTENSION_SOURCE_DIRS
  ${Colada_SOURCE_DIR}/Modules/Loadable/Axes
  ${Colada_SOURCE_DIR}/Modules/Loadable/Captions
  ${Colada_SOURCE_DIR}/Modules/Loadable/SceneFilter
  ${Colada_SOURCE_DIR}/Modules/Loadable/SpatialReference
  ${Colada_SOURCE_DIR}/Modules/Loadable/Zoom
  ${Colada_SOURCE_DIR}/Modules/Scripted/Home
  ${Colada_SOURCE_DIR}/Modules/Scripted/GeoObjectInfo
  # ${Colada_SOURCE_DIR}/Modules/Extensions/Seismic
  # ${Colada_SOURCE_DIR}/Modules/Extensions/Maps
  # ${Colada_SOURCE_DIR}/Modules/Extensions/Wells
  )

# Add remote extension source directories

# SlicerOpenIGTLink
#set(extension_name "SlicerOpenIGTLink")
#set(${extension_name}_SOURCE_DIR "${CMAKE_BINARY_DIR}/${extension_name}")
#FetchContent_Populate(${extension_name}
#  SOURCE_DIR     ${${extension_name}_SOURCE_DIR}
#  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/openigtlink/SlicerOpenIGTLink.git
#  GIT_TAG        2b92f7b1ffe02403109b671f28424e8770e902a0
#  GIT_PROGRESS   1
#  QUIET
#  )
#list(APPEND Slicer_EXTENSION_SOURCE_DIRS ${${extension_name}_SOURCE_DIR})

# SlicerDebuggingTools
set(extension_name "SlicerDebuggingTools")
set(${extension_name}_SOURCE_DIR "${CMAKE_BINARY_DIR}/${extension_name}")
FetchContent_Populate(${extension_name}
  SOURCE_DIR     ${${extension_name}_SOURCE_DIR}
  GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/SlicerRt/SlicerDebuggingTools
  GIT_TAG        8f5436b8519e10163fd88a86f5641fe70fc6697d
  GIT_PROGRESS   1
  QUIET
  )
list(APPEND Slicer_EXTENSION_SOURCE_DIRS ${${extension_name}_SOURCE_DIR})

# SlicerJupyter
# set(extension_name "SlicerJupyter")
# set(${extension_name}_SOURCE_DIR "${CMAKE_BINARY_DIR}/${extension_name}")
# FetchContent_Populate(${extension_name}
#   SOURCE_DIR     ${${extension_name}_SOURCE_DIR}
#   GIT_REPOSITORY ${EP_GIT_PROTOCOL}://github.com/Slicer/SlicerJupyter
#   GIT_TAG        b282a2c819da1e3e4fe1ab6c419e2299f976720d
#   GIT_PROGRESS   1
#   QUIET
#   )
# list(APPEND Slicer_EXTENSION_SOURCE_DIRS ${${extension_name}_SOURCE_DIR})


include(${CMAKE_SOURCE_DIR}/CMake/ColadaMacroEditSourceCode.cmake)
coladaMacroEditcpp(
  TARGET MRMLLogic
  TARGET_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/Logic/CMakeLists.txt
  SRC_FILE ${CMAKE_SOURCE_DIR}/EditSourceCode/MRMLLogic/vtkMRMLSliceLogic.cxx
  DEST_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/Logic/vtkMRMLSliceLogic.cxx
  CXX_STANDARD ${CMAKE_CXX_STANDARD}
  # OVERWRITE
  OLDFILE_DELETE
)

coladaMacroEditcpp(
  TARGET MRMLCore
  TARGET_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/Core/CMakeLists.txt
  SRC_FILE ${CMAKE_SOURCE_DIR}/EditSourceCode/MRMLCore/vtkMRMLAbstractViewNode.cxx
  DEST_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/Core/vtkMRMLAbstractViewNode.cxx
  CXX_STANDARD ${CMAKE_CXX_STANDARD}
  # OVERWRITE
  OLDFILE_DELETE
)

coladaMacroEditcpp(
  TARGET MRMLDisplayableManager
  TARGET_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/DisplayableManager/CMakeLists.txt
  SRC_FILE ${CMAKE_SOURCE_DIR}/EditSourceCode/MRMLDisplayableManager/vtkMRMLCameraWidget.cxx
  DEST_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Libs/MRML/DisplayableManager/vtkMRMLCameraWidget.cxx
  CXX_STANDARD ${CMAKE_CXX_STANDARD}
  # OVERWRITE
  OLDFILE_DELETE
)

coladaMacroEditcpp(
  TARGET qSlicerViewControllersModule
  TARGET_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Modules/Loadable/ViewControllers/CMakeLists.txt
  SRC_FILE ${CMAKE_SOURCE_DIR}/EditSourceCode/ViewControllers/qSlicerViewControllersModule.cxx
  DEST_FILE ${CMAKE_BINARY_DIR}/slicersources-src/Modules/Loadable/ViewControllers/qSlicerViewControllersModule.cxx
  CXX_STANDARD ${CMAKE_CXX_STANDARD}
  # OVERWRITE
  OLDFILE_DELETE
)

# Add Slicer sources
add_subdirectory(${slicersources_SOURCE_DIR} ${slicersources_BINARY_DIR})
