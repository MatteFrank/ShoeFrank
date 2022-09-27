# CMake for FOOT
# Ch. Finck

#----------------------------------------------------------------------------
# Globals 
#
MESSAGE("libs:")
#
add_subdirectory(GenFit)
if (TOE_DIR)
  add_subdirectory(TOE)
endif(TOE_DIR)
add_subdirectory(src)
