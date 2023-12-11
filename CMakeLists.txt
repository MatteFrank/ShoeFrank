# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Check cmake version is compatible
#
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
set(CMAKE_MACOSX_RPATH 0)
#------------------------------------------------------------------------------
# Project Name 
#
project (FOOT)
#
MESSAGE("${PROJECT_NAME}:")

#------------------------------------------------------------------------------
# If not specified, set the build type to Debug as default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

#------------------------------------------------------------------------------
# option Geant4
option(Geant4_DIR "Include G4 simulation" OFF)

#------------------------------------------------------------------------------
# option TOE
option(TOE_DIR "Include TOE reconstruction" OFF)

#------------------------------------------------------------------------------
# option GenFit
option(GENFIT_DIR "Include GENFIT_DIR reconstruction" ON)

#------------------------------------------------------------------------------
# option Ancillary
option(ANC_DIR "Include ancillary" OFF)

#------------------------------------------------------------------------------
# option copying files into build directory
option(FILECOPY "Files copying" OFF)

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
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Libraries/lib)

#----------------------------------------------------------------------------
# Set src libraries name
#
Set(TATOE "TATOEbase")
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
Set(TATOE "TATOEbase")
Set(TAGF  "TAGFbase")
Set(TAFO  "TAGfoot")

Set(TAMP "TAMPbase")
Set(TAPL "TAPLbase")
Set(TACE "TACEbase")
Set(TAPW "TAPWbase")
Set(TANE "TANEbase")
Set(TAPXI "TAPXIbase")

add_definitions(-std=c++17)
add_definitions(-Wno-shadow)

#----------------------------------------------------------------------------
# Set src includes
#
set(TATOE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TATOE}")
set(TAG_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAG}")
set(TADAQ_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TADAQ}")
set(TAAPI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAAPI}")
set(TADI_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TADI}")
set(TAIR_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAIR}")
set(TAST_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAST}")
set(TABM_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TABM}")
set(TAVT_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAVT}")
set(TAIT_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAIT}")
set(TAMSD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAMSD}")
set(TATW_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TATW}")
set(TACA_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TACA}")
set(TAMC_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAMC}")
set(TAED_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAED}")
set(TATOE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TATOE}")
set(TAGF_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAGF}")
set(TAFO_INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/${TAFO}")

set(TAPL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TAPL}")
set(TACE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TACE}")
set(TAPW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TAPW}")
set(TANE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TANE}")
set(TAPXI_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TAPXI}")
set(TAMP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ancillary/${TAMP}")

#----------------------------------------------------------------------------
#  Print Geant4 directory and find REC part of FOOT
#
find_package(REC)
if (REC_FOUND)
#  	include_directories(${REC_INCLUDE_DIR})
else()
   MESSAGE(" --> REC has NOT been found")
endif()

#----------------------------------------------------------------------------
# GenFit
#
if (GENFIT_DIR)
  SET(GENFIT_INCLUDE_DIR
      ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/core/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/eventDisplay/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/fields/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/finitePlanes/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/fitters/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/GBL/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/measurements/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/trackReps/include/
		${CMAKE_CURRENT_SOURCE_DIR}/Libraries/GenFit/utilities/include/
      )

  set (GENFIT_LIBRARIES  genfit2)
  add_compile_definitions(GENFIT_FLAG=1)
endif(GENFIT_DIR)

#----------------------------------------------------------------------------
# TOE
#
if (TOE_DIR)
  SET(TOE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/TOE/)
  SET(TOE_LIBRARIES  TOE)
  add_compile_definitions(TOE_FLAG=1)
endif(TOE_DIR)

#----------------------------------------------------------------------------
## for rootlogon.C
##
string(REPLACE ";" " -I" GENFIT_INCLUDE_ROOT "${GENFIT_INCLUDE_DIR}")
string(PREPEND  GENFIT_INCLUDE_ROOT "-I")

#----------------------------------------------------------------------------
# Globals 
#
add_subdirectory(Libraries)
add_subdirectory(Reconstruction)
add_subdirectory(Simulation)
if (Geant4_DIR)
  add_subdirectory(G4Simulation)
endif(Geant4_DIR)
add_subdirectory(Analysis)
if (ANC_DIR)
   add_subdirectory(Ancillary)
endif(ANC_DIR)

