# - Finds REC instalation
# This module sets up REC information 
# It defines:
# REC_FOUND          If the REC is found
# REC_INCLUDE_DIR    PATH to the include directory
# REC_LIBRARIES      Most common libraries
# REC_LIBRARY_DIR    PATH to the library directory 

# check that library path for rec exist

set(BUILD_REC "${CMAKE_BINARY_DIR}/libs")
set(FOOT_REC "${CMAKE_SOURCE_DIR}/libs/src")
set(FOOT_LEVEL0 "${CMAKE_SOURCE_DIR}/Reconstruction/level0")
set(FOOT_FULLREC "${CMAKE_SOURCE_DIR}/Reconstruction/fullrec")
set(REC_INCLUDE_DIR ${FOOT_REC}/TAGbase ${FOOT_REC}/TADIbase ${FOOT_REC}/TAMCbase ${FOOT_REC}/TASTbase ${FOOT_REC}/TABMbase ${FOOT_REC}/TAVTbase ${FOOT_REC}/TAITbase ${FOOT_REC}/TAMSDbase ${FOOT_REC}/TACAbase ${FOOT_REC}/TATWbase ${FOOT_REC}/TAEDbase ${FOOT_REC}/TAGfoot)


set(REC_LIBRARIES -lTAGbase -lTADIbase -lTAMCbase -lTASTbase -lTABMbase -lTAMCbase -lTAVTbase -lTAITbase -lTAMSDbase -lTACAbase -lTATWbase -lTAEDbase  -lTAGfoot -L${BUILD_REC}/lib)

set(SIM_REC_LIBRARIES TAGbase TADIbase TAMCbase TASTbase TABMbase TAMCbase TAVTbase TAITbase TAMSDbase TACAbase TATWbase TAEDbase TAGfoot)
set(REC_LIBRARY_DIR ${FOOT_REC}/lib)

set(REC_GEOMAPS_DIR ${FOOT_LEVEL0}/geomaps)
set(REC_CONFIG_DIR  ${FOOT_LEVEL0}/config)
  
if(NOT REC_FIND_QUIETLY)
  message(" --> Found REC in ${FOOT_REC}")
 endif()
 
set(REC_FOUND TRUE)


