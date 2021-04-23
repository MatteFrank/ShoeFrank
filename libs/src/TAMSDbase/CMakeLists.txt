# Ch. Finck
#
project (TAMSDbase)
#
MESSAGE("  src/${PROJECT_NAME}:")
#
#
set(LIBNAME ${PROJECT_NAME})
#
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES ${TAG} ${TAVT} ${TAMC})
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})

#
# headers / dictionnary / sources 
# 
# so far no automatic methods for computing which class has a dictionnary or not
# manual method : base file name separated in two lists with different treatment.
#
set( with_dictionnaries
   TAMSDntuRaw
   TAMSDntuHit
   TAMSDhit
   TAMSDntuPoint
   TAMSDparMap
   TAMSDparGeo
   TAMSDparConf
   TAMSDdigitizer
   TAMSDactNtuHitMC
   TAMSDntuCluster
   TAMSDcluster
   TAMSDactNtuCluster
   TAMSDactNtuPoint
)

set( without_dictionnaries
)

# add includes : root + current + dependencies
#
include_directories(${ROOT_INCLUDE_DIR})
include_directories(${GENFIT_INCLUDE_DIR})
include_directories(${TAG_INCLUDE_DIR})
include_directories(${TAMC_INCLUDE_DIR})
include_directories(${TAVT_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#
link_directories(${ROOT_LIBRARY_DIR})
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#
# headers / linkdef etc ... 
#
set(headers "")
set(sources "")
set(dictionnaries "")
set(linkdefs "")
#
# set headers/ etc and generate dictionnary for the files which require it 
#
foreach( f ${with_dictionnaries} )
  set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cxx)
  set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hxx)
endforeach()

if (ROOT6_USED)
  ROOT6_GENERATE_DICTIONARY(G_${LIBNAME} ${headers} MODULE ${LIBNAME} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}.LinkDef.h OPTIONS "")
else()
  ROOT5_GENERATE_DICTIONARY(G_${LIBNAME} ${all_headers} LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}.LinkDef.h OPTIONS "")
endif()

set(sources ${sources} ${PROJECT_BINARY_DIR}/G_${LIBNAME}.cxx)

#
foreach( f ${without_dictionnaries} )
	set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cxx)
	set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hxx)
endforeach()
#

#MESSAGE("headers ${headers}")
#MESSAGE("sources ${sources}")
#MESSAGE("dictionnaries ${dictionnaries}")
#MESSAGE("ROOT ${ROOT_INCLUDE_DIR}")
#MESSAGE("ROOT ${ROOT_LIBRARY_DIR}")

#----------------------------------------------------------------------------
#
# Use root functions to generate the library and the root map file 
#
ROOT_LINKER_LIBRARY(${LIBNAME} ${sources} CMAKENOEXPORT LIBRARIES ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})
if (!ROOT6_USED)
  ROOT_GENERATE_ROOTMAP(${LIBNAME} LINKDEF ${linkdefs} DEPENDENCIES ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES})
endif()
#
# Install lib and headers 
#
install(TARGETS ${LIBNAME}  RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib
							ARCHIVE DESTINATION lib
							COMPONENT libraries)
