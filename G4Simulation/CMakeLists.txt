# Ch. Finck
#
project (G4Simulation)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


#----------------------------------------------------------------------------
#  Print Geant4 directory
#
message(" --> Found Geant4 in ${Geant4_DIR}")

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

if(Geant4_gdml_FOUND)
		ADD_DEFINITIONS(-DHAS_GDML)
		MESSAGE(" --> GDML is ON")	
endif()

#----------------------------------------------------------------------------
# Set the output paths
#
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

#----------------------------------------------------------------------------
# Set libraries name
#
Set(TCG   "TCGbase")
Set(TCST  "TCSTbase")
Set(TCBM  "TCBMbase")
Set(TCEM  "TCEMbase")
Set(TCVT  "TCVTbase")
Set(TCIT  "TCITbase")
Set(TCMSD "TCMSDbase")
Set(TCCA  "TCCAbase")
Set(TCTW  "TCTWbase")
Set(TCFO  "TCFObase")

#----------------------------------------------------------------------------
# Set includes
#
set(TCG_INCLUDE_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/${TCG}")
set(TCST_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCST}")
set(TCBM_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCBM}")
set(TCEM_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCEM}")
set(TCVT_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCVT}")
set(TCIT_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCIT}")
set(TCMSD_INCLUDE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/${TCMSD}")
set(TCCA_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCCA}")
set(TCTW_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCTW}")
set(TCFO_INCLUDE_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/${TCFO}")

#----------------------------------------------------------------------------
# Copy geomaps + config file and vis geant macros in working sim directory
#
if (FILECOPY)
 file(COPY ${FOOT_LEVEL0}/cammaps/ DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation/cammaps/)
 file(COPY ${FOOT_LEVEL0}/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation/geomaps/)
 file(COPY ${FOOT_LEVEL0}/config/  DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/README DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation/)
 execute_process(COMMAND ln -sf ${CMAKE_BINARY_DIR}/Reconstruction/data/ ${CMAKE_BINARY_DIR}/G4Simulation)
endif(FILECOPY)

#
file(COPY ${CMAKE_SOURCE_DIR}/G4Simulation/macros/ DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation/)

#----------------------------------------------------------------------------
# Copy rootlogon
#
file(COPY ${CMAKE_BINARY_DIR}/Reconstruction/level0/rootlogon.C DESTINATION ${CMAKE_BINARY_DIR}/G4Simulation)

#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${REC_LIBRARIES} ${TCG} ${TCST} ${TCBM} ${TCEM} ${TCVT} ${TCIT} ${TCCA} ${TCTW} ${TCMSD} ${TCFO})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${GENFIT_LIBRARIES})

# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${REC_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

include_directories(${TCG_INCLUDE_DIR})
include_directories(${TCST_INCLUDE_DIR})
include_directories(${TCBM_INCLUDE_DIR})
include_directories(${TCEM_INCLUDE_DIR})
include_directories(${TCVT_INCLUDE_DIR})
include_directories(${TCIT_INCLUDE_DIR})
include_directories(${TCCA_INCLUDE_DIR})
include_directories(${TCTW_INCLUDE_DIR})
include_directories(${TCMSD_INCLUDE_DIR})
include_directories(${TCFO_INCLUDE_DIR})


#----------------------------------------------------------------------------
# Add sub directories
#
add_subdirectory(TCGbase)
add_subdirectory(TCSTbase)
add_subdirectory(TCBMbase)
add_subdirectory(TCEMbase)
add_subdirectory(TCVTbase)
add_subdirectory(TCITbase)
add_subdirectory(TCMSDbase)
add_subdirectory(TCCAbase)
add_subdirectory(TCTWbase)
add_subdirectory(TCFObase)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(TAGsimulation TAGsimulation.cxx)
#
target_link_libraries(TAGsimulation ${EXTRA_EXTERNAL_LIBRARIES}  ${EXTRA_INTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS TAGsimulation DESTINATION bin)


