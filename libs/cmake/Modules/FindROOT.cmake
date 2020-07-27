# - Finds ROOT instalation
# This module sets up ROOT information 
# It defines:
# ROOT_FOUND          If the ROOT is found
# ROOT_INCLUDE_DIR    PATH to the include directory
# ROOT_LIBRARIES      Most common libraries
# ROOT_LIBRARY_DIR    PATH to the library directory 


set(ROOT6_USED FALSE)

find_program(ROOT_CONFIG_EXECUTABLE root-config
  PATHS $ENV{ROOTSYS}/bin)

if(NOT ROOT_CONFIG_EXECUTABLE)
  set(ROOT_FOUND FALSE)
else()    
  set(ROOT_FOUND TRUE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix 
    OUTPUT_VARIABLE ROOTSYS 
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
    OUTPUT_VARIABLE ROOT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  execute_process(
    COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
    OUTPUT_VARIABLE ROOT_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  string(COMPARE LESS ${ROOT_VERSION} "6.02/00" version)

   if (version)
   message("Support for Root 5")
   execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --evelibs
      OUTPUT_VARIABLE ROOT_LIBRARIES
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  else()
    set(ROOT6_USED TRUE)
    execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --evelibs
      OUTPUT_VARIABLE ROOT_LIBRARIES
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()

 # Add XML library
 set( ROOT_LIBRARIES ${ROOT_LIBRARIES} -lXMLIO -lXMLParser)
 string(REPLACE ";" " " ROOT_LIBRARIES "${ROOT_LIBRARIES}")

  set(ROOT_LIBRARY_DIR ${ROOTSYS}/lib)
  
  # Make variables changeble to the advanced user
  mark_as_advanced(ROOT_CONFIG_EXECUTABLE)

  if(NOT ROOT_FIND_QUIETLY)
    message(" --> Found ROOT ${ROOT_VERSION} in ${ROOTSYS}")
  endif()
endif()


include(CMakeMacroParseArguments)
find_program(ROOTCINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)
find_program(GENREFLEX_EXECUTABLE genreflex PATHS $ENV{ROOTSYS}/bin)
find_package(GCCXML)



#----------------------------------------------------------------------------
# function ROOT5_GENERATE_DICTIONARY( dictionary   
#                                    header1 header2 ... 
#                                    LINKDEF linkdef1 ... 
#                                    OPTIONS opt1...)
function(ROOT5_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "LINKDEF;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get the list of include directories------------------
  set(includedirsall ${ALL_INCLUDES})
   
  foreach( d ${includedirsall})    
   set(includedirs ${includedirs} -I${d})
  endforeach()

 # message("${includedirs}")
  #---Get LinkDef.h file------------------------------------
  set(linkdefs)
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(linkdefs ${linkdefs} ${f})
    else() 
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()
  #---call rootcint------------------------------------------
  add_custom_command(OUTPUT ${dictionary}.cxx ${dictionary}.h
                     COMMAND ${ROOTCINT_EXECUTABLE} -cint -f  ${dictionary}.cxx 
                                          -c ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs} 
                     DEPENDS ${headerfiles} ${linkdefs})
endfunction()

#----------------------------------------------------------------------------
# function REFLEX_GENERATE_DICTIONARY(dictionary   
#                                     header1 header2 ... 
#                                     SELECTION selectionfile ... 
#                                     OPTIONS opt1...)
function(REFLEX_GENERATE_DICTIONARY dictionary)  
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "SELECTION;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        set(headerfiles ${headerfiles} ${f})
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get Selection file------------------------------------
  if(IS_ABSOLUTE ${ARG_SELECTION})
    set(selectionfile ${ARG_SELECTION})
  else() 
    set(selectionfile ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SELECTION})
  endif()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  set(includedirs) 
  foreach( d ${incdirs})    
    set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get preprocessor definitions--------------------------
  get_directory_property(defs COMPILE_DEFINITIONS)
  foreach( d ${defs})    
   set(definitions ${definitions} -D${d})
  endforeach()
  #---Nanes and others---------------------------------------
  set(gensrcdict ${dictionary}.cpp)
  if(MSVC)
    set(gccxmlopts "--gccxmlopt=\"--gccxml-compiler cl\"")
  else()
    #set(gccxmlopts "--gccxmlopt=\'--gccxml-cxxflags -m64 \'")
    set(gccxmlopts)
  endif()  
  #set(rootmapname ${dictionary}Dict.rootmap)
  #set(rootmapopts --rootmap=${rootmapname} --rootmap-lib=${libprefix}${dictionary}Dict)
  #---Check GCCXML and get path-----------------------------
  if(GCCXML)
    get_filename_component(gccxmlpath ${GCCXML} PATH)
  else()
    message(WARNING "GCCXML not found. Install and setup your environment to find 'gccxml' executable")
  endif()
  #---Actual command----------------------------------------
  add_custom_command(OUTPUT ${gensrcdict} ${rootmapname}     
                     COMMAND ${GENREFLEX_EXECUTABLE} ${headerfiles} -o ${gensrcdict} ${gccxmlopts} ${rootmapopts} --select=${selectionfile}
                             --gccxmlpath=${gccxmlpath} ${ARG_OPTIONS} ${includedirs} ${definitions}
                     DEPENDS ${headerfiles} ${selectionfile})
endfunction()

include(CMakeParseArguments)

#---------------------------------------------------------------------------------------------------
#---ROOT6_GENERATE_DICTIONARY( dictionary headerfiles MODULE module DEPENDENCIES dep1 dep2
#                                                    STAGE1 LINKDEF linkdef OPTIONS opt1 opt2 ...)
#---------------------------------------------------------------------------------------------------
function(ROOT6_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "STAGE1;MULTIDICT" "MODULE" "LINKDEF;OPTIONS;DEPENDENCIES" ${ARGN})

  #---roottest compability---------------------------------
  if(CMAKE_ROOTTEST_DICT)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR})
    set(libprefix "")
  endif()

  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files inc/${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/inc/" ""  rheaderfiles "${headerfiles}")
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  if(CMAKE_PROJECT_NAME STREQUAL ROOT)
    set(includedirs -I${CMAKE_SOURCE_DIR}
                    -I${CMAKE_BINARY_DIR}/include)
  elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    set(includedirs -I${CMAKE_CURRENT_SOURCE_DIR}/inc)
  endif()
  foreach( d ${incdirs})
   set(includedirs ${includedirs} -I${d})
  endforeach()
  list(REMOVE_DUPLICATES includedirs)

  #---Get the list of definitions---------------------------
  get_directory_property(defs COMPILE_DEFINITIONS)
  foreach( d ${defs})
   if((NOT d MATCHES "=") AND (NOT d MATCHES "^[$]<.*>$")) # avoid generator expressions
     set(definitions ${definitions} -D${d})
   endif()
  endforeach()
  #---Get LinkDef.h file------------------------------------
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(_linkdef ${_linkdef} ${f})
    else()
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(_linkdef ${_linkdef} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(_linkdef ${_linkdef} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()

  #---Build the names for library, pcm and rootmap file ----
  get_filename_component(dict_base_name ${dictionary} NAME_WE)
  if(dict_base_name MATCHES "^G__")
    string(SUBSTRING ${dictionary} 3 -1 deduced_arg_module)
  else()
    set(deduced_arg_module ${dict_base_name})
  endif()

  #---Set the library output directory-----------------------
  if(DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(library_output_dir ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
  else()
    set(library_output_dir ${CMAKE_CURRENT_BINARY_DIR}/lib)
  endif()


  if(ARG_MODULE)
    set(library_name ${libprefix}${ARG_MODULE}${libsuffix})
    if(ARG_MULTIDICT)
      set(newargs -s ${library_output_dir}/${library_name} -multiDict)
      set(pcm_name ${library_output_dir}/${libprefix}${ARG_MODULE}_${dictionary}_rdict.pcm)
      set(rootmap_name ${library_output_dir}/${libprefix}${deduced_arg_module}.rootmap)
    else()
      set(newargs -s ${library_output_dir}/${library_name})
      set(pcm_name ${library_output_dir}/${libprefix}${ARG_MODULE}_rdict.pcm)
      set(rootmap_name ${library_output_dir}/${libprefix}${ARG_MODULE}.rootmap)
    endif()
  else()
    set(library_name ${libprefix}${deduced_arg_module}${libsuffix})
    set(pcm_name ${dictionary}_rdict.pcm)
    set(rootmap_name ${library_output_dir}/${libprefix}${deduced_arg_module}.rootmap)
  endif()


  if(CMAKE_ROOTTEST_NOROOTMAP)
    set(rootmapname )
    set(rootmapargs )
  else()
    set(rootmapargs -rml ${library_name} -rmf ${rootmap_name})
  endif()

  #---Get the library and module dependencies-----------------
  if(ARG_DEPENDENCIES)
    foreach(dep ${ARG_DEPENDENCIES})
      set(newargs ${newargs} -m  ${libprefix}${dep}_rdict.pcm)
    endforeach()
  endif()

  #---what rootcling command to use--------------------------
  if(ARG_STAGE1)
    set(command rootcling_tmp)
    set(pcm_name)
  else()
    if(CMAKE_PROJECT_NAME STREQUAL ROOT)
      set(command rootcling -rootbuild)
      set(ROOTCINTDEP rootcling)
    else()
      set(command $ENV{ROOTSYS}/bin/rootcling)
    endif()
  endif()

  #---call rootcint------------------------------------------
  add_custom_command(OUTPUT ${dictionary}.cxx ${pcm_name} ${rootmap_name}
                     COMMAND ${command} -f  ${dictionary}.cxx ${newargs} ${rootmapargs}
                                        ${ARG_OPTIONS} ${definitions} ${includedirs} ${rheaderfiles} ${_linkdef}
                     DEPENDS ${headerfiles} ${_linkdef} ${ROOTCINTDEP})
  get_filename_component(dictname ${dictionary} NAME)

  #---roottest compability
  if(CMAKE_ROOTTEST_DICT OR (NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY))
    add_custom_target(${dictname} DEPENDS ${dictionary}.cxx)
  else()
    add_custom_target(${dictname} DEPENDS ${dictionary}.cxx)

    set_property(GLOBAL APPEND PROPERTY ROOT_DICTIONARY_TARGETS ${dictname})
    set_property(GLOBAL APPEND PROPERTY ROOT_DICTIONARY_FILES ${CMAKE_CURRENT_BINARY_DIR}/${dictionary}.cxx)

#    if(ARG_STAGE1)
#      install(FILES ${rootmap_name}
#                    DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
#    else()
#      install(FILES ${pcm_name} ${rootmap_name}
#                    DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT libraries)
#    endif()
  endif()
endfunction()

  
