# - Finds REC instalation
# This module sets up REC information 
# It defines:
# REC_FOUND          If the REC is found
# REC_INCLUDE_DIR    PATH to the include directory
# REC_LIBRARIES      Most common libraries
# REC_LIBRARY_DIR    PATH to the library directory 

# check that library path for rec exist

set(BUILD_REC "${CMAKE_BINARY_DIR}/libs")
set(FOOT_SRC "${CMAKE_SOURCE_DIR}/libs/src")
set(FOOT_REC "${CMAKE_SOURCE_DIR}/Reconstruction")
set(FOOT_LEVEL0 "${CMAKE_SOURCE_DIR}/Reconstruction/level0")
set(FOOT_FULLREC "${CMAKE_SOURCE_DIR}/Reconstruction/fullrec")
set(REC_INCLUDE_DIR ${FOOT_SRC}/TAGbase ${FOOT_SRC}/TADIbase ${FOOT_SRC}/TAMCbase ${FOOT_SRC}/TASTbase ${FOOT_SRC}/TABMbase ${FOOT_SRC}/TAVTbase ${FOOT_SRC}/TAITbase ${FOOT_SRC}/TAMSDbase ${FOOT_SRC}/TACAbase ${FOOT_SRC}/TATWbase ${FOOT_SRC}/TAEDbase ${FOOT_SRC}/TAGfoot)


set(REC_LIBRARIES -lTAGbase -lTADIbase -lTAMCbase -lTASTbase -lTABMbase -lTAMCbase -lTAVTbase -lTAITbase -lTAMSDbase -lTACAbase -lTATWbase -lTAEDbase  -lTAGfoot -L${BUILD_REC}/lib)

set(SIM_REC_LIBRARIES TAGbase TADIbase TAMCbase TASTbase TABMbase TAMCbase TAVTbase TAITbase TAMSDbase TACAbase TATWbase TAEDbase TAGfoot)
set(REC_LIBRARY_DIR ${FOOT_SRC}/lib)

set(REC_GEOMAPS_DIR ${FOOT_LEVEL0}/geomaps)
set(REC_CONFIG_DIR  ${FOOT_LEVEL0}/config)
  
if(NOT REC_FIND_QUIETLY)
  message(" --> Found REC in ${FOOT_SRC}")
 endif()
 
set(REC_FOUND TRUE)


