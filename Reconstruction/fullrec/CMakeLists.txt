# Ch. Finck
#
project (fullrec)
#
MESSAGE("  ${PROJECT_NAME}:")
#
#
set(LIBNAME RecoTools)

#----------------------------------------------------------------------------
# Copy rootlogon
#
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/macros/rootlogon.C.cmake" "${CMAKE_BINARY_DIR}/Reconstruction/fullrec/rootlogon.C"  @ONLY)

#----------------------------------------------------------------------------
# Copy geomaps + config (tmp solution)
#
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/geomaps/ DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/geomaps/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../level0/config/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/config/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data/  DESTINATION ${CMAKE_BINARY_DIR}/Reconstruction/fullrec/data/)

#----------------------------------------------------------------------------
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TADI}  ${TADAQ}  ${TAAPI} ${TAVT} ${TAIT} ${TAMSD} ${TAIR} ${TABM} ${TATW} ${TACA} ${TAST} ${TAMC} ${TAED} ${TAFO})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES} ${GENFIT_LIBRARIES})

#----------------------------------------------------------------------------
# headers / dictionnary / sources 
# 
# so far no automatic methods for computing which class has a dictionnary or not
# manual method : base file name separated in two lists with different treatment.
#
set( with_dictionnaries
   RecoTools
)

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
include_directories(${TAFO_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#
# headers / linkdef etc ... 
#
set(headers "")
set(all_headers "")
set(sources "")
set(dictionnaries "")
set(linkdefs "")
#
# set headers/ etc and generate dictionnary for the files which require it 
#
foreach( f ${with_dictionnaries} )
  set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cc)
  set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.h)
endforeach()

file(GLOB all_headers ${TAG_INCLUDE_DIR}/*.hxx)
set(all_headers ${all_headers} ${headers})


if (ROOT6_USED)
  ROOT6_GENERATE_DICTIONARY(G_${LIBNAME} ${headers} MODULE ${LIBNAME} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}.LinkDef.h OPTIONS "")
else()
  ROOT5_GENERATE_DICTIONARY(G_${LIBNAME} ${all_headers} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}.LinkDef.h OPTIONS "")
endif()

set(sources ${sources} ${PROJECT_BINARY_DIR}/G_${LIBNAME}.cxx)

#----------------------------------------------------------------------------
#
# Use root functions to generate the library and the root map file 
#

ROOT_LINKER_LIBRARY(${LIBNAME} ${sources} CMAKENOEXPORT LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})
if (!ROOT6_USED)
  ROOT_GENERATE_ROOTMAP(${LIBNAME} LINKDEF ${linkdefs} DEPENDENCIES ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})
endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlb RecoL0.cc)
#
target_link_libraries(DecodeGlb ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} ${LIBNAME} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlb DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlob DecodeGlb.cc)
#
target_link_libraries(DecodeGlob ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlob DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlobMC DecodeGlbMC.cc)
#
target_link_libraries(DecodeGlobMC ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlobMC DESTINATION bin)


#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlbToe DecodeGlbToe.cc)
#
target_link_libraries(DecodeGlbToe ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlbToe DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(DecodeGlbToeMC DecodeGlbToeMC.cc)
#
target_link_libraries(DecodeGlbToeMC ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DecodeGlbToeMC DESTINATION bin)
