# CMake for FOOT
# Ch. Finck

#----------------------------------------------------------------------------
# Globals 
#
MESSAGE("libs:")
#
if (GENFIT_DIR)
  add_subdirectory(GenFit)
endif(GENFIT_DIR)

if (TOE_DIR)
  add_subdirectory(TOE)
endif(TOE_DIR)

add_subdirectory(src)
