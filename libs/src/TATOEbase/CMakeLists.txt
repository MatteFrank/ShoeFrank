# Ch. Finck
#
project (TATOEbase)
#
MESSAGE("  src/${PROJECT_NAME}:")
#
#
set(LIBNAME ${PROJECT_NAME})
#
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADI} ${TADAQ} ${TAAPI} ${TAST} ${TAVT} ${TAIT} ${TAEM} ${TAMSD} ${TAIR} ${TABM} ${TATW} ${TACA} ${TAIR} ${TAMC} ${TAED} ${GENFIT_LIBRARIES} ${TOE_LIBRARIES})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})

#
# headers / sources 
# 
# so far no automatic methods for computing which class has a dictionnary or not
# manual method : base file name separated in two lists with different treatment.
#

set( without_dictionnaries
    TATOEact
    TATOEutilities
)

# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${TAG_INCLUDE_DIR})
include_directories(${TADI_INCLUDE_DIR})
include_directories(${TADAQ_INCLUDE_DIR})
include_directories(${TAAPI_INCLUDE_DIR})
include_directories(${TAST_INCLUDE_DIR})
include_directories(${TAIR_INCLUDE_DIR})
include_directories(${TABM_INCLUDE_DIR})
include_directories(${TAVT_INCLUDE_DIR})
include_directories(${TAIT_INCLUDE_DIR})
include_directories(${TAMC_INCLUDE_DIR})
include_directories(${TAMSD_INCLUDE_DIR})
include_directories(${TAIR_INCLUDE_DIR})
include_directories(${TATW_INCLUDE_DIR})
include_directories(${TACA_INCLUDE_DIR})
include_directories(${TAED_INCLUDE_DIR})
include_directories(${TOE_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#
# headers / linkdef etc ... 
#
set(headers "")
set(sources "")

#
foreach( f ${without_dictionnaries} )
	set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cxx)
	set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hxx)
endforeach()
#

#----------------------------------------------------------------------------
#
# Generate the library
#
add_library(${LIBNAME} SHARED  ${sources} )
#
set_target_properties(${LIBNAME} PROPERTIES ${ROOT_LIBRARY_PROPERTIES})
#
target_link_libraries(${LIBNAME} ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#
# Install lib and headers
#
install(TARGETS ${LIBNAME}  RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib
							ARCHIVE DESTINATION lib
							COMPONENT libraries)
