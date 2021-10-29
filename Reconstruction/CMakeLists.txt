# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Project Name 
#
project (Reconstruction)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Globals 
#
add_subdirectory(level0)
add_subdirectory(fullrec)

#----------------------------------------------------------------------------
# Copy data
#
if (FILECOPY)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/data/)
endif(FILECOPY)
