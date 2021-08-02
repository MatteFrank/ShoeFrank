# Ch. Finck
#
project (level0)
#
MESSAGE("  ${PROJECT_NAME}:")
#
#
set(LIBNAME LocalReco)

#----------------------------------------------------------------------------
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/level0/rootlogon.C"  @ONLY)

#----------------------------------------------------------------------------
# Copy geomaps + config + macro
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/geomaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/macros/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cammaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/cammaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/calib/   DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/calib/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/README   DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/level0/)
 execute_process(COMMAND ln -sf ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/data/ ${CMAKE_BINARY_DIR}/Reconstruction/level0/)
endif(FILECOPY)


#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADAQ} ${TAAPI} ${TAST} ${TADI} ${TAVT} ${TAIT} ${TAMSD} ${TABM} ${TATW} ${TACA} ${TAIR} ${TAMC} ${TAED} ${TATOE} ${TAGF} ${TAFO})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES} ${GENFIT_LIBRARIES})

# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${TAG_INCLUDE_DIR})

include_directories(${TADAQ_INCLUDE_DIR})
include_directories(${TAAPI_INCLUDE_DIR})

include_directories(${TAST_INCLUDE_DIR})
include_directories(${TAIR_INCLUDE_DIR})
include_directories(${TABM_INCLUDE_DIR})
include_directories(${TADI_INCLUDE_DIR})
include_directories(${TAVT_INCLUDE_DIR})
include_directories(${TAIT_INCLUDE_DIR})
include_directories(${TAMC_INCLUDE_DIR})
include_directories(${TAMSD_INCLUDE_DIR})
include_directories(${TATW_INCLUDE_DIR})
include_directories(${TACA_INCLUDE_DIR})
include_directories(${TAED_INCLUDE_DIR})
include_directories(${TAFO_INCLUDE_DIR})
include_directories(${TATOE_INCLUDE_DIR})
include_directories(${TAGF_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeRaw DecodeRaw.cc)
#
#target_compile_options( DecodeRaw PUBLIC -g -I/usr/local/Cellar/gperftools/2.8/include/ )
target_link_libraries(DecodeRaw ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
#target_link_options(DecodeRaw PUBLIC  -L/usr/local/Cellar/gperftools/2.8/lib/ -lprofiler)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeRaw DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeMC DecodeMC.cc)
#
target_link_libraries(DecodeMC ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeMC DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(CalibrateTof CalibrateTof.cc)
#
target_link_libraries(CalibrateTof ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS CalibrateTof DESTINATION bin)
#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(CalibrateBm CalibrateBm.cc)
#
target_link_libraries(CalibrateBm ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS CalibrateBm DESTINATION bin)
#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(AnalyzeBmStd AnalyzeBmStd.cc)
#
target_link_libraries(AnalyzeBmStd ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS AnalyzeBmStd DESTINATION bin)
