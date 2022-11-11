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
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/WC/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Ancillary/WC/rootlogon.C"  @ONLY)
