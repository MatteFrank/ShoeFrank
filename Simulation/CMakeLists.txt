# Ch. Finck
#
project (Simulation)
#
MESSAGE("${PROJECT_NAME}:")
#

#----------------------------------------------------------------------------
#  Print Geant4 directory and find REC part of FOOT
#
find_package(REC)
if (REC_FOUND)
   	include_directories(${REC_INCLUDE_DIR})
else()
   MESSAGE(" --> REC has NOT been found")
endif()

#----------------------------------------------------------------------------
# Copy
#
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/link_FOOT.sh DESTINATION ${CMAKE_BINARY_DIR}/Simulation/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/link_FOOT_mag.sh DESTINATION ${CMAKE_BINARY_DIR}/Simulation/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/foot.inp DESTINATION ${CMAKE_BINARY_DIR}/Simulation/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/ROUTINES/  DESTINATION ${CMAKE_BINARY_DIR}/Simulation/ROUTINES)

execute_process(COMMAND ln -sf ${FOOT_FULLREC}/data/  ${CMAKE_BINARY_DIR}/Simulation)
execute_process(COMMAND ln -sf ${FOOT_LEVEL0}/geomaps/  ${CMAKE_BINARY_DIR}/Simulation)
execute_process(COMMAND ln -sf ${FOOT_LEVEL0}/config/  ${CMAKE_BINARY_DIR}/Simulation)

#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${REC_LIBRARIES})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES} ${GENFIT_LIBRARIES})


# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${REC_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#----------------------------------------------------------------------------
# Add the executable, and link it to the libraries
#
add_executable(makeGeo MakeGeo.cxx)
#
target_link_libraries(makeGeo ${EXTRA_EXTERNAL_LIBRARIES}  ${EXTRA_INTERNAL_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS makeGeo DESTINATION bin)


#----------------------------------------------------------------------------
# Add the executable, and link it to the libraries
#
add_executable(Txt2Root Txt2Root.cxx)
#
target_link_libraries(Txt2Root ${EXTRA_EXTERNAL_LIBRARIES}  ${EXTRA_INTERNAL_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Txt2Root DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the libraries
#
add_executable(Txt2NtuRoot Txt2NtuRoot.cxx)
#
target_link_libraries(Txt2NtuRoot ${EXTRA_EXTERNAL_LIBRARIES}  ${EXTRA_INTERNAL_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Txt2NtuRoot DESTINATION bin)
