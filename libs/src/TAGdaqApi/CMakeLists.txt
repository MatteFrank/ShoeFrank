# Ch. Finck
#
project (TAGdaqApi)
#
MESSAGE("  src/${PROJECT_NAME}:")
#
#
set(LIBNAME ${PROJECT_NAME})
#
# internal and external dependencies
#
set( EXTRA_INTERNAL_LIBRARIES "")
set( EXTRA_EXTERNAL_LIBRARIES "")

#
# headers / dictionnary / sources 
# 
# so far no automatic methods for computing which class has a dictionnary or not
# manual method : base file name separated in two lists with different treatment.
#
	
set( files
   BaseFragment
   DECardEvent
   EventReaderAndChecker
   TDCEvent
   WDEvent
   DAQFileHeader
   EmptyEvent
   InfoEvent
   TrgEvent
   EventReader
   RemoteEvent
   fADCEvent
)

# add includes : current 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
#
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
#
# headers etc ... 
#
set(headers "")
set(sources "")
#
foreach( f ${files} )
	set(sources ${sources} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.cpp)
	set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.hh)
endforeach()
#

#MESSAGE("headers ${headers}")
#MESSAGE("sources ${sources}")

#----------------------------------------------------------------------------
#
# Use root functions to generate the library
#
ROOT_LINKER_LIBRARY(${LIBNAME} ${sources} LIBRARIES )
#		
# Install lib and headers 
#
install(TARGETS ${LIBNAME}  RUNTIME DESTINATION bin
							LIBRARY DESTINATION lib
							ARCHIVE DESTINATION lib
							COMPONENT libraries)		
#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(CheckDAQFile CheckMain.cpp)
#
target_link_libraries(CheckDAQFile ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} ${LIBNAME} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS CheckDAQFile DESTINATION bin)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Genfit and root libraries
#
add_executable(readDAQFile EventReaderMain.cpp)
#
target_link_libraries(readDAQFile ${EXTRA_INTERNAL_LIBRARIES} ${EXTRA_EXTERNAL_LIBRARIES} ${LIBNAME} )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS readDAQFile DESTINATION bin)
