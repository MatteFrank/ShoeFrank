# CMake for FOOT
# Ch. Finck

#----------------------------------------------------------------------------
# Globals 
#
MESSAGE("Libraries:")
#
if (GENFIT_DIR)
  add_subdirectory(GenFit)
endif(GENFIT_DIR)

if (TOE_DIR)
  add_subdirectory(TOE)
endif(TOE_DIR)

add_subdirectory(TAGdaq)
add_subdirectory(TAGdaqApi)
add_subdirectory(TAGbase)
add_subdirectory(TADIbase)
add_subdirectory(TASTbase)
add_subdirectory(TAIRbase)
add_subdirectory(TABMbase)
add_subdirectory(TAVTbase)
add_subdirectory(TAITbase)
add_subdirectory(TAMSDbase)
add_subdirectory(TAMCbase)
add_subdirectory(TATWbase)
add_subdirectory(TACAbase)
add_subdirectory(TAEDbase)
if (TOE_DIR)
   add_subdirectory(TATOEbase)
endif(TOE_DIR)
if (GENFIT_DIR)
   add_subdirectory(TAGFbase)
endif(GENFIT_DIR)
add_subdirectory(TAGfoot)
