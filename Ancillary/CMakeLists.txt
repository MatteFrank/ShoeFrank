# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Project Name 
#
project (Ancillary)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Set src libraries name
#
Set(TAPL "TAPLbase")
Set(TACE "TACEbase")

#----------------------------------------------------------------------------
# Set src includes
#
set(TAPL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${TAPL}")
set(TACE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${TACE}")

#----------------------------------------------------------------------------
# SRC
#
add_subdirectory(TAPLbase)
add_subdirectory(TACEbase)

#----------------------------------------------------------------------------
# Copy geomaps + config + macro
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/WC/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Ancillary/WC/geomaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/WC/config/  DESTINATION ${CMAKE_BINARY_DIR}/Ancillary/WC/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/WC/macros/  DESTINATION ${CMAKE_BINARY_DIR}/Ancillary/WC/  PATTERN "*.cmake" EXCLUDE)
 endif(FILECOPY)
