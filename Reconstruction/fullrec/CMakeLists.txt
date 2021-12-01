# Ch. Finck
#
project (fullrec)
#
MESSAGE("  ${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/fullrec/rootlogon.C"  @ONLY)

#----------------------------------------------------------------------------
# Copy geomaps + config + calib + cammaps (tmp solution)
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/geomaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/config/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/cammaps/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/cammaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/calib/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/calib/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/macros/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/)
 execute_process(COMMAND ln -sf ${CMAKE_BINARY_DIR}/Reconstruction/data/ ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/)
endif(FILECOPY)

#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADI}  ${TADAQ}  ${TAAPI} ${TAVT} ${TAIT} ${TAMSD} ${TAIR} ${TABM} ${TATW} ${TACA} ${TAST} ${TAMC} ${TAED} ${TATOE} ${TAFO})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES} ${GENFIT_LIBRARIES})

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

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlb DecodeGlb.cc)
#
target_link_libraries(DecodeGlb ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlb DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlbToe DecodeGlbToe.cc)
#
#target_compile_options( DecodeGlbToe PUBLIC -g -I/usr/local/Cellar/gperftools/2.8/include/ )
target_compile_options( DecodeGlbToe PUBLIC -march=native )
target_link_libraries(DecodeGlbToe ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})
#target_link_options(DecodeGlbToe PUBLIC  -L/usr/local/Cellar/gperftools/2.8/lib/ -lprofiler)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlbToe DESTINATION bin)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
if(DEFINED OTC )
    if( "${OTC}" STREQUAL ON )
    add_executable(OptimizeToeCuts OptimizeToeCuts.cc)
    target_compile_options( OptimizeToeCuts PUBLIC -march=native -ffast-math  )
    target_link_libraries(OptimizeToeCuts ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS OptimizeToeCuts DESTINATION bin)

    endif()
endif()
