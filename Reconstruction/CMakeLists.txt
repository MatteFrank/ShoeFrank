# CMake for FOOT
# Ch. Finck

#------------------------------------------------------------------------------
# Project Name
#
project (Reconstruction)
#
MESSAGE("${PROJECT_NAME}:")

#----------------------------------------------------------------------------
# Copy data
#
if (FILECOPY)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/data/)
endif(FILECOPY)


#----------------------------------------------------------------------------
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/rootlogon.C"  @ONLY)

#----------------------------------------------------------------------------
# Copy geomaps + config + macro
#
if (FILECOPY)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/geomaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/config/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/macros/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/  PATTERN "*.cmake" EXCLUDE)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/cammaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/cammaps/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/calib/   DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/calib/)
 file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/README   DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/)
endif(FILECOPY)

#----------------------------------------------------------------------------
# Night build script
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/nightbuild.csh.cmake"      "${CMAKE_BINARY_DIR}/Reconstruction/nightbuild.csh"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/nightbuild_ui10.csh.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_ui10.csh"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/nightbuild_li.csh.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_li.csh"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/nightbuild_tier1.sh.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_tier1.sh"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cron.tab.cmake"            "${CMAKE_BINARY_DIR}/Reconstruction/cron.tab"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/cron.tab_ui10.cmake"       "${CMAKE_BINARY_DIR}/Reconstruction/cron_ui10.tab"  @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/scripts/CompileMacro.py.cmake"     "${CMAKE_BINARY_DIR}/Reconstruction/CompileMacro.py"  @ONLY)

execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/Reconstruction/nightbuild.csh)
execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_ui10.csh)
execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_li.csh)
execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/Reconstruction/nightbuild_tier1.sh)
execute_process(COMMAND chmod u+x ${CMAKE_BINARY_DIR}/Reconstruction/CompileMacro.py)


#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADAQ} ${TAAPI} ${TAST} ${TADI} ${TAVT} ${TAIT} ${TAMSD} ${TABM} ${TATW} ${TACA} ${TAIR} ${TAMC} ${TAED} ${TAFO} ${TANA})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})

if (GENFIT_DIR)
   set(EXTRA_INTERNAL_LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${GENFIT_LIBRARIES} ${TAGF})
endif(GENFIT_DIR)

if (TOE_DIR)
   set(EXTRA_INTERNAL_LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${TATOE} ${TOE_LIBRARIES})
endif(TOE_DIR)

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

include_directories(${TANA_INCLUDE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
#add_executable(DecodeRaw DecodeRaw.cc)
#
#target_compile_options( DecodeRaw PUBLIC -g -I/usr/local/Cellar/gperftools/2.8/include/ )
#target_link_libraries(DecodeRaw ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
#target_link_options(DecodeRaw PUBLIC  -L/usr/local/Cellar/gperftools/2.8/lib/ -lprofiler)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
add_executable(DecodeWD DecodeWD.cc)
target_link_libraries(DecodeWD ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
install(TARGETS DecodeWD DESTINATION bin)

add_executable(DecodeCA DecodeCA.cc)
target_link_libraries(DecodeCA ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} )
install(TARGETS DecodeCA DESTINATION bin)

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
add_executable(ConvertNtuple ConvertNtuple.cc)
#
target_link_libraries(ConvertNtuple ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ConvertNtuple DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlbAna DecodeGlbAna.cc)
#
target_link_libraries(DecodeGlbAna ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlbAna DESTINATION bin)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlbAnalysis DecodeGlbAnalysis.cc)
#
target_link_libraries(DecodeGlbAnalysis ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlbAnalysis DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
if (TOE_DIR)
if(DEFINED OTC )
    if( "${OTC}" STREQUAL ON )
    add_executable(OptimizeToeCuts OptimizeToeCuts.cc)
    target_compile_options( OptimizeToeCuts PUBLIC -ffast-math  )
    target_link_libraries(OptimizeToeCuts ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS OptimizeToeCuts DESTINATION bin)

    endif()
endif()
endif (TOE_DIR)
