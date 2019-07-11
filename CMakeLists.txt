# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Check cmake version is compatible
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_MACOSX_RPATH 0)
#------------------------------------------------------------------------------
# Project Name 
#
project (FOOT)
#
MESSAGE("${PROJECT_NAME}:")

#------------------------------------------------------------------------------
# option Geant4
option(Geant4_DIR "Include G4 simulation" OFF)

#------------------------------------------------------------------------------
# Enforce an out-of-source builds before anything else
#
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(STATUS "FOOT requires an out-of-source build.")
    message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
    message(STATUS "CMakeCache.txt")
    message(STATUS "CMakeFiles")
    message(STATUS "Once these files are removed, create a separate directory")
    message(STATUS "and run CMake from there")
    message(FATAL_ERROR "in-source build detected")
endif()

#----------------------------------------------------------------------------
# Add specific modules
#
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/libs/cmake/Modules/")

MESSAGE(" --> Including cmake additional path: ${CMAKE_MODULE_PATH}")

#----------------------------------------------------------------------------
# Include macro for root
#
include(RootNewMacros)
include(GenFit)

#----------------------------------------------------------------------------
# Try and Find Root
#
find_package(ROOT)
if (ROOT_FOUND)
   # add ROOT includes and libs
      include_directories(${ROOT_INCLUDE_DIR})
   else()
      MESSAGE(" --> root has NOT been found")
endif()

#----------------------------------------------------------------------------
# Copy setup files
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setupFOOT.sh.cmake" "${CMAKE_BINARY_DIR}/setupFOOT.sh"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/setupFOOT.csh.cmake" "${CMAKE_BINARY_DIR}/setupFOOT.csh"  @ONLY)


#----------------------------------------------------------------------------
# Set the output paths
#
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/libs/lib)

#----------------------------------------------------------------------------
# Set src libraries name
#
Set(TAG   "TAGbase")
Set(TADAQ "TAGdaq")
Set(TAAPI "TAGdaqApi")
Set(TADI  "TADIbase")
Set(TAIR  "TAIRbase")
Set(TAST  "TASTbase")
Set(TABM  "TABMbase")
Set(TAVT  "TAVTbase")
Set(TAIT  "TAITbase")
Set(TAMSD "TAMSDbase")
Set(TATW  "TATWbase")
Set(TACA  "TACAbase")
Set(TAMC  "TAMCbase")
Set(TAED  "TAEDbase")
Set(TAFO  "TAGfoot")
add_definitions(-std=c++14)

#----------------------------------------------------------------------------
# Set src includes
#
set(TAG_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAG}")
set(TADAQ_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TADAQ}")
set(TAAPI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAAPI}")
set(TADI_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TADI}")
set(TAIR_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAIR}")
set(TAST_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAST}")
set(TABM_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TABM}")
set(TAVT_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAVT}")
set(TAIT_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAIT}")
set(TAMSD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAMSD}")
set(TATW_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TATW}")
set(TACA_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TACA}")
set(TAMC_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAMC}")
set(TAED_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAED}")
set(TAFO_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/libs/src/${TAFO}")

#----------------------------------------------------------------------------
# GenFit
#
SET(GENFIT_INCLUDE_DIR
      ${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/core/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/eventDisplay/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/fields/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/finitePlanes/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/fitters/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/GBL/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/measurements/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/trackReps/include/
		${CMAKE_CURRENT_SOURCE_DIR}/libs/GenFit/utilities/include/
      )

set (GENFIT_LIBRARIES  genfit2)

#----------------------------------------------------------------------------
## for rootlogon.C
##
string(REPLACE ";" " -I" GENFIT_INCLUDE_ROOT "${GENFIT_INCLUDE_DIR}")
string(PREPEND  GENFIT_INCLUDE_ROOT "-I")

#----------------------------------------------------------------------------
# Globals 
#
add_subdirectory(libs)
add_subdirectory(Reconstruction)
add_subdirectory(Simulation)
if (Geant4_DIR)
  add_subdirectory(G4Simulation)
endif(Geant4_DIR)

