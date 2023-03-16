
# define output directories
SET( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin" )
SET( LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib" )
MARK_AS_ADVANCED( EXECUTABLE_OUTPUT_PATH )
MARK_AS_ADVANCED( LIBRARY_OUTPUT_PATH )

# what happens when `make install` is invoked:
# set default install prefix to project root directory
# instead of the cmake default /usr/local
IF( CMAKE_INSTALL_PREFIX STREQUAL "/usr/local" )
SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}" )
ENDIF()

# write this variable to cache
SET( CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Where to install ${PROJECT_NAME}" FORCE )


# create symbolic lib target for calling library targets
MACRO( ADD_SHARED_LIBRARY _name )
    ADD_LIBRARY( ${_name} SHARED ${ARGN} )

    set (GENFIT_PROPERTY SUFFIX .so)
    # change lib_target properties
    SET_TARGET_PROPERTIES( ${_name} PROPERTIES ${GENFIT_PROPERTY})

ENDMACRO( ADD_SHARED_LIBRARY )


