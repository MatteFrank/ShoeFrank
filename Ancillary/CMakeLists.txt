# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Project Name 
#
project (Ancillary)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# SRC
#
add_subdirectory(TAPLbase)
add_subdirectory(TACEbase)
add_subdirectory(TAPWbase)
add_subdirectory(TANEbase)
add_subdirectory(TAPXIbase)
add_subdirectory(TAMPbase)


#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TAVT} ${TAMP} ${TAPXI} ${TAPW} ${TANE} ${TAPL} ${TACE})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})

# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})

include_directories(${TAG_INCLUDE_DIR})
include_directories(${TAVT_INCLUDE_DIR})
include_directories(${TAMP_INCLUDE_DIR})
include_directories(${TAPXI_INCLUDE_DIR})
include_directories(${TAPW_INCLUDE_DIR})
include_directories(${TANE_INCLUDE_DIR})
include_directories(${TAPL_INCLUDE_DIR})
include_directories(${TACE_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#----------------------------------------------------------------------------
# Copy macro
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/macros/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/  PATTERN "*.cmake" EXCLUDE)
endif(FILECOPY)

#----------------------------------------------------------------------------
# Add the executable, and link it to root libraries
#
add_executable(DecodeMPix DecodeMpix.cc)
#
target_link_libraries(DecodeMPix ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeMPix DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to root libraries
#
add_executable(DecodeWC DecodeWC.cc)
#
target_link_libraries(DecodeWC ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeWC DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to root libraries
#
add_executable(DecodePix DecodePix.cc)
#
target_link_libraries(DecodePix ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodePix DESTINATION bin)
