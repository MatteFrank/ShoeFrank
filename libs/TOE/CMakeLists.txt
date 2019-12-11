# Ch. Finck
#
project (TOE)
#
MESSAGE("  ${PROJECT_NAME}:")
#
#
set(LIBNAME ${PROJECT_NAME})
#
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES "")
set( EXTRA_EXTERNAL_LIBRARIES ${ROOT_LIBRARIES})
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")


# add includes :current + dependencies
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#
# headers
#
set(headers "")
#
foreach( f ${without_dictionnaries} )
	set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hpp)
endforeach()

#----------------------------------------------------------------------------
#
# Install lib and headers 
#
add_library(${LIBNAME} INTERFACE)

target_include_directories(${LIBNAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${LIBNAME}  RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib
							ARCHIVE DESTINATION lib
							COMPONENT libraries)
