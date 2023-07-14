# CMake for Qapivi/rec/TAVTbase
# Ch. Finck
#
project (TCLINMbase)
#
MESSAGE("  G4Simulation/${PROJECT_NAME}:")
#
#
set(LIBNAME ${PROJECT_NAME})
#
set( EXTRA_INTERNAL_LIBRARIES ${TCST} ${TCPL} ${TCCE} ${TCPW})

#
# headers / dictionnary / sources 
# 
# so far no automatic methods for computing which class has a dictionnary or not
# manual method : base file name separated in two lists with different treatment.
#
set( Qapivi_file
    TCLINMgeometryConstructor
    TCLINMbaseEventAction
    TCLINMeventAction
    TCLINMrunAction
    TCLINMtrackingAction
)

# add includes : root + current + dependencies 
#
# set headers/ etc and generate dictionnary for the files which require it 
#
foreach( f ${Qapivi_file} )
  set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cxx)
  set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hxx)
endforeach()

include_directories(${TCG_INCLUDE_DIR})
include_directories(${TCPL_INCLUDE_DIR})
include_directories(${TCCE_INCLUDE_DIR})
include_directories(${TCPW_INCLUDE_DIR})

message("[${TCPL_INCLUDE_DIR}]")

#----------------------------------------------------------------------------
#
# Use root functions to generate the library and the root map file 
#
#
# Install lib and headers 
#
add_library(${LIBNAME} SHARED ${sources})
add_dependencies(${LIBNAME} ${SIM_REC_LIBRARIES})
#
set_target_properties(${LIBNAME} PROPERTIES SUFFIX ".so")
#
target_link_libraries( ${LIBNAME}  ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${ANC_LIBRARIES} ${REC_LIBRARIES} ${EXTRA_INTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install libraries
#
install(TARGETS ${LIBNAME}  RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT libraries)
