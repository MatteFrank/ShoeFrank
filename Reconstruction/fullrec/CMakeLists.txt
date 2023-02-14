# Ch. Finck
#
project (fullrec)
#
MESSAGE("  ${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/../level0/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/fullrec/rootlogon.C"  @ONLY)

#----------------------------------------------------------------------------
# Copy geomaps + config + calib + cammaps (tmp solution)
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/geomaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/config/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/cammaps/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/cammaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/calib/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/calib/)
 execute_process(COMMAND ln -sf ${CMAKE_BINARY_DIR}/Reconstruction/data/ ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/)
endif(FILECOPY)

#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADI}  ${TADAQ}  ${TAAPI} ${TAVT} ${TAIT} ${TAMSD} ${TAIR} ${TABM} ${TATW} ${TACA} ${TAST} ${TAMC} ${TAED} ${TAFO})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})

if (GENFIT_DIR)
 set(EXTRA_INTERNAL_LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${GENFIT_LIBRARIES} ${TAGF})
endif(GENFIT_DIR)

if (TOE_DIR)
   set(EXTRA_INTERNAL_LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${TOE_LIBRARIES} ${TATOE})
endif(TOE_DIR)
# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${TAG_INCLUDE_DIR})
include_directories(${TADI_INCLUDE_DIR})
include_directories(${TADAQ_INCLUDE_DIR})
include_directories(${TAAPI_INCLUDE_DIR})
include_directories(${TAST_INCLUDE_DIR})
include_directories(${TABM_INCLUDE_DIR})
include_directories(${TAVT_INCLUDE_DIR})
include_directories(${TAIT_INCLUDE_DIR})
include_directories(${TAIR_INCLUDE_DIR})
include_directories(${TAMC_INCLUDE_DIR})
include_directories(${TAMSD_INCLUDE_DIR})
include_directories(${TATW_INCLUDE_DIR})
include_directories(${TACA_INCLUDE_DIR})
include_directories(${TAED_INCLUDE_DIR})
include_directories(${TATOE_INCLUDE_DIR})
include_directories(${TAFO_INCLUDE_DIR})
include_directories(${TAGF_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#----------------------------------------------------------------------------
# Copy README
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/README DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/)
endif(FILECOPY)
