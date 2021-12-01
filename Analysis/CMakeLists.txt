# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Project Name 
#
project (Analysis)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# SRC
#
add_subdirectory(TPGbase)

#----------------------------------------------------------------------------
# Copy data
#
#if (FILECOPY)
#  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/data/)
#endif(FILECOPY)
