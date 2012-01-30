# - SWIG module for CMake
# Defines the following macros:
#   SWIG_ADD_MODULE(name language [ files ])
#     - Define swig module with given name and specified language
#   SWIG_LINK_LIBRARIES(name [ libraries ])
#     - Link libraries to swig module
# All other macros are for internal use only.
# To get the actual name of the swig module,
# use: ${SWIG_MODULE_${name}_REAL_NAME}.
# Set Source files properties such as CPLUSPLUS and SWIG_FLAGS to specify
# special behavior of SWIG. Also global CMAKE_SWIG_FLAGS can be used to add
# special flags to all swig calls.
# Another special variable is CMAKE_SWIG_OUTDIR, it allows one to specify 
# where to write all the swig generated module (swig -outdir option)
# The name-specific variable SWIG_MODULE_<name>_EXTRA_DEPS may be used
# to specify extra dependencies for the generated modules.
# If the source file generated by swig need some special flag you can use
# SET_SOURCE_FILES_PROPERTIES( ${swig_generated_file_fullname}
#        PROPERTIES COMPILE_FLAGS "-bla")


#=============================================================================
# Copyright 2004-2009 Kitware, Inc.
# Copyright 2009 Mathieu Malaterre <mathieu.malaterre@gmail.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distributed this file outside of CMake, substitute the full
#  License text for the above reference.)

SET(SWIG_CXX_EXTENSION "cxx")
SET(SWIG_EXTRA_LIBRARIES "")

SET(SWIG_PYTHON_EXTRA_FILE_EXTENSION "py")

#
# For given swig module initialize variables associated with it
#
MACRO(SWIG_MODULE_INITIALIZE name language)
  STRING(TOUPPER "${language}" swig_uppercase_language)
  STRING(TOLOWER "${language}" swig_lowercase_language)
  SET(SWIG_MODULE_${name}_LANGUAGE "${swig_uppercase_language}")
  SET(SWIG_MODULE_${name}_SWIG_LANGUAGE_FLAG "${swig_lowercase_language}")

  IF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xUNKNOWNx$")
    MESSAGE(FATAL_ERROR "SWIG Error: Language \"${language}\" not found")
  ENDIF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xUNKNOWNx$")

  SET(SWIG_MODULE_${name}_REAL_NAME "${name}")
  IF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xPYTHONx$")
    # when swig is used without the -interface it will produce in the module.py
    # a 'import _modulename' statement, which implies having a corresponding 
    # _modulename.so (*NIX), _modulename.pyd (Win32).
    SET(SWIG_MODULE_${name}_REAL_NAME "_${name}")
  ENDIF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xPYTHONx$")
  IF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xPERLx$")
    SET(SWIG_MODULE_${name}_EXTRA_FLAGS "-shadow")
  ENDIF("x${SWIG_MODULE_${name}_LANGUAGE}x" MATCHES "^xPERLx$")
ENDMACRO(SWIG_MODULE_INITIALIZE)

#
# For a given language, input file, and output file, determine extra files that
# will be generated. This is internal swig macro.
#

MACRO(SWIG_GET_EXTRA_OUTPUT_FILES language outfiles generatedpath infile)
  GET_SOURCE_FILE_PROPERTY(SWIG_GET_EXTRA_OUTPUT_FILES_module_basename
    ${infile} SWIG_MODULE_NAME)
  IF(SWIG_GET_EXTRA_OUTPUT_FILES_module_basename STREQUAL "NOTFOUND")
    GET_FILENAME_COMPONENT(SWIG_GET_EXTRA_OUTPUT_FILES_module_basename "${infile}" NAME_WE)
  ENDIF(SWIG_GET_EXTRA_OUTPUT_FILES_module_basename STREQUAL "NOTFOUND")
  FOREACH(it ${SWIG_${language}_EXTRA_FILE_EXTENSION})
    SET(${outfiles} ${${outfiles}}
      "${generatedpath}/${SWIG_GET_EXTRA_OUTPUT_FILES_module_basename}.${it}")
  ENDFOREACH(it)
ENDMACRO(SWIG_GET_EXTRA_OUTPUT_FILES)

#
# Take swig (*.i) file and add proper custom commands for it
#
MACRO(SWIG_ADD_SOURCE_TO_MODULE name outfiles infile)
#message ( STATUS "aqui 0 - infile=${infile}") 
  SET(swig_full_infile ${infile})
#  GET_FILENAME_COMPONENT(swig_source_file_path "${infile}" PATH)
  GET_FILENAME_COMPONENT(swig_source_file_path "${infile}" REALPATH)
  GET_FILENAME_COMPONENT(swig_source_file_name_we "${infile}" NAME_WE)
  GET_SOURCE_FILE_PROPERTY(swig_source_file_generated ${infile} GENERATED)
  GET_SOURCE_FILE_PROPERTY(swig_source_file_cplusplus ${infile} CPLUSPLUS)
  GET_SOURCE_FILE_PROPERTY(swig_source_file_flags ${infile} SWIG_FLAGS)
  IF("${swig_source_file_flags}" STREQUAL "NOTFOUND")
    SET(swig_source_file_flags "")
  ENDIF()
  SET(swig_source_file_fullname "${infile}")
  IF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_SOURCE_DIR}")
#message ( STATUS "aqui 1 - swig_source_file_path=${swig_source_file_path}")
    STRING(REGEX REPLACE "^${CMAKE_CURRENT_SOURCE_DIR}[/\\]" "" swig_source_file_relative_path "${swig_source_file_path}")
#message ( STATUS "aqui 1 - swig_source_file_relative_path=${swig_source_file_relative_path}")
  ELSE()
    IF(${swig_source_file_path} MATCHES "^${CMAKE_CURRENT_BINARY_DIR}")
      STRING(REGEX REPLACE "^${CMAKE_CURRENT_BINARY_DIR}[/\\]" "" swig_source_file_relative_path "${swig_source_file_path}")
#message ( STATUS "aqui 2 - swig_source_file_relative_path=${swig_source_file_relative_path}")
      SET(swig_source_file_generated 1)
    ELSE()
      STRING(REGEX REPLACE "^${CMAKE_CURRENT_BINARY_DIR}[/\\]" "" swig_source_file_relative_path "${swig_source_file_path}")
#message ( STATUS "aqui 3 - swig_source_file_relative_path=${swig_source_file_relative_path}")
      SET(swig_source_file_generated 1)
      SET(swig_source_file_relative_path "${swig_source_file_path}")
      IF(swig_source_file_generated)
        SET(swig_source_file_fullname "${CMAKE_CURRENT_BINARY_DIR}/${infile}")
#message ( STATUS "aqui 3.1   swig_source_file_relative_path=${swig_source_file_relative_path}    infile=${infile}")
        SET(swig_source_file_fullname "${infile}")
      ELSE()
        SET(swig_source_file_fullname "${CMAKE_CURRENT_SOURCE_DIR}/${infile}")
#message ( STATUS "aqui 3.2   swig_source_file_relative_path=${swig_source_file_relative_path}    infile=${infile}")
        SET(swig_source_file_fullname "${infile}")
      ENDIF()
    ENDIF()
  ENDIF()

  SET(swig_generated_file_fullname "${CMAKE_CURRENT_BINARY_DIR}")
  IF(swig_source_file_relative_path)
#message ( STATUS "aqui 4.0.0 - swig_source_file_relative_path=${swig_source_file_relative_path}" )
#    STRING(REGEX REPLACE "(^/\\.\\.)?" ".." swig_source_file_relative_path "${swig_source_file_relative_path}")
#message ( STATUS "aqui 4.0.1 - swig_source_file_relative_path=${swig_source_file_relative_path}" )
#    STRING(REGEX REPLACE "^\\\.\\." ".." swig_source_file_relative_path "${swig_source_file_relative_path}")
#message ( STATUS "aqui 4.1 - swig_source_file_relative_path=${swig_source_file_relative_path}  swig_source_file_relative_path=${swig_source_file_relative_path}" )
    swig_find_common_path ( common_path "${swig_generated_file_fullname}" "${swig_source_file_relative_path}" )
#message ( STATUS "aqui 4 - swig_generated_file_fullname=${swig_generated_file_fullname}  swig_source_file_relative_path=${swig_source_file_relative_path} common_path=${common_path}" )
    if ( common_path )
#message ( STATUS "aqui 5 - swig_source_file_relative_path=${swig_source_file_relative_path}" )
       STRING ( REGEX REPLACE "${common_path}[/\\]" "" swig_source_file_relative_path ${swig_source_file_relative_path} )
#message ( STATUS "aqui 6 - swig_source_file_relative_path=${swig_source_file_relative_path}" )
    else ()
    endif ()
    SET(swig_generated_file_fullname "${swig_generated_file_fullname}/${swig_source_file_relative_path}")
    # it may not exist, so create it:
    file(MAKE_DIRECTORY ${swig_generated_file_fullname})
  ENDIF()
  # If CMAKE_SWIG_OUTDIR was specified then pass it to -outdir
  IF(CMAKE_SWIG_OUTDIR)
    SET(swig_outdir ${CMAKE_SWIG_OUTDIR})
    # it may not exist, so create it:
    file(MAKE_DIRECTORY ${swig_outdir})
  ELSE(CMAKE_SWIG_OUTDIR)
    SET(swig_outdir ${CMAKE_CURRENT_BINARY_DIR})
    # it may not exist, so create it:
    file(MAKE_DIRECTORY ${swig_outdir})
  ENDIF(CMAKE_SWIG_OUTDIR)
  SWIG_GET_EXTRA_OUTPUT_FILES(${SWIG_MODULE_${name}_LANGUAGE}
    swig_extra_generated_files
    "${swig_outdir}"
    "${infile}")
  SET(swig_generated_file_fullname
    "${swig_generated_file_fullname}/${swig_source_file_name_we}")
  # add the language into the name of the file (i.e. TCL_wrap)
  # this allows for the same .i file to be wrapped into different languages
  SET(swig_generated_file_fullname
    "${swig_generated_file_fullname}${SWIG_MODULE_${name}_LANGUAGE}_wrap")

  IF(swig_source_file_cplusplus)
    SET(swig_generated_file_fullname
      "${swig_generated_file_fullname}.${SWIG_CXX_EXTENSION}")
  ELSE(swig_source_file_cplusplus)
    SET(swig_generated_file_fullname
      "${swig_generated_file_fullname}.c")
  ENDIF(swig_source_file_cplusplus)

  #MESSAGE(STATUS "Full path to source file: ${swig_source_file_fullname}")
  #MESSAGE(STATUS "Full path to the output file: ${swig_generated_file_fullname}")
  GET_DIRECTORY_PROPERTY(cmake_include_directories INCLUDE_DIRECTORIES)
  SET(swig_include_dirs)
  FOREACH(it ${cmake_include_directories})
    SET(swig_include_dirs ${swig_include_dirs} "-I${it}")
  ENDFOREACH(it)

  SET(swig_special_flags)
  # default is c, so add c++ flag if it is c++
  IF(swig_source_file_cplusplus)
    SET(swig_special_flags ${swig_special_flags} "-c++")
  ENDIF(swig_source_file_cplusplus)
  SET(swig_extra_flags)
  IF(SWIG_MODULE_${name}_EXTRA_FLAGS)
    SET(swig_extra_flags ${swig_extra_flags} ${SWIG_MODULE_${name}_EXTRA_FLAGS})
  ENDIF(SWIG_MODULE_${name}_EXTRA_FLAGS)
  ADD_CUSTOM_COMMAND(
    OUTPUT "${swig_generated_file_fullname}" ${swig_extra_generated_files}
    # Let's create the ${swig_outdir} at execution time, in case dir contains $(OutDir)
    COMMAND ${CMAKE_COMMAND} -E make_directory ${swig_outdir}
    COMMAND "${SWIG_EXECUTABLE}"
    ARGS "-${SWIG_MODULE_${name}_SWIG_LANGUAGE_FLAG}"
    ${swig_source_file_flags}
    ${CMAKE_SWIG_FLAGS}
    -outdir ${swig_outdir}
    ${swig_special_flags}
    ${swig_extra_flags}
    ${swig_include_dirs}
    -o "${swig_generated_file_fullname}"
    "${swig_source_file_fullname}"
    MAIN_DEPENDENCY "${swig_source_file_fullname}"
    DEPENDS ${SWIG_MODULE_${name}_EXTRA_DEPS}
    COMMENT "Swig source") 
  SET_SOURCE_FILES_PROPERTIES("${swig_generated_file_fullname}" ${swig_extra_generated_files}
    PROPERTIES GENERATED 1)
  SET(${outfiles} "${swig_generated_file_fullname}" ${swig_extra_generated_files})
ENDMACRO(SWIG_ADD_SOURCE_TO_MODULE)


#-------------------------------------------------------------------
#
# Create Swig module
#
MACRO(SWIG_ADD_MODULE name language)
  SWIG_MODULE_INITIALIZE(${name} ${language})
  SET(swig_dot_i_sources)
  SET(swig_other_sources)
  FOREACH(it ${ARGN})
    IF(${it} MATCHES ".*\\.i$" OR ${it} MATCHES ".*\\.swg$")
      SET(swig_dot_i_sources ${swig_dot_i_sources} "${it}")
    ELSE()
      SET(swig_other_sources ${swig_other_sources} "${it}")
    ENDIF()
  ENDFOREACH(it)

  SET(swig_generated_sources)
  FOREACH(it ${swig_dot_i_sources})
    SWIG_ADD_SOURCE_TO_MODULE(${name} swig_generated_source ${it})
    SET(swig_generated_sources ${swig_generated_sources} "${swig_generated_source}")
  ENDFOREACH(it)
  GET_DIRECTORY_PROPERTY(swig_extra_clean_files ADDITIONAL_MAKE_CLEAN_FILES)
  SET_DIRECTORY_PROPERTIES(PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${swig_extra_clean_files};${swig_generated_sources}")
  ADD_LIBRARY(${SWIG_MODULE_${name}_REAL_NAME}
    MODULE
    ${swig_generated_sources}
    ${swig_other_sources})
  STRING(TOLOWER "${language}" swig_lowercase_language)
  IF ("${swig_lowercase_language}" STREQUAL "java")
    IF (APPLE)
        # In java you want:
        #      System.loadLibrary("LIBRARY");
        # then JNI will look for a library whose name is platform dependent, namely
        #   MacOS  : libLIBRARY.jnilib
        #   Windows: LIBRARY.dll
        #   Linux  : libLIBRARY.so
        SET_TARGET_PROPERTIES (${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES SUFFIX ".jnilib")
      ENDIF (APPLE)
  ENDIF ("${swig_lowercase_language}" STREQUAL "java")
  IF ("${swig_lowercase_language}" STREQUAL "python")
    # this is only needed for the python case where a _modulename.so is generated
    SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES PREFIX "")
    # Python extension modules on Windows must have the extension ".pyd"
    # instead of ".dll" as of Python 2.5.  Older python versions do support
    # this suffix.
    # http://docs.python.org/whatsnew/ports.html#SECTION0001510000000000000000
    # <quote>
    # Windows: .dll is no longer supported as a filename extension for extension modules.
    # .pyd is now the only filename extension that will be searched for.
    # </quote>
    IF(WIN32 AND NOT CYGWIN)
      SET_TARGET_PROPERTIES(${SWIG_MODULE_${name}_REAL_NAME} PROPERTIES SUFFIX ".pyd")
    ENDIF(WIN32 AND NOT CYGWIN)
  ENDIF ("${swig_lowercase_language}" STREQUAL "python")
ENDMACRO(SWIG_ADD_MODULE)

#
# Like TARGET_LINK_LIBRARIES but for swig modules
#
MACRO(SWIG_LINK_LIBRARIES name)
  IF(SWIG_MODULE_${name}_REAL_NAME)
    TARGET_LINK_LIBRARIES(${SWIG_MODULE_${name}_REAL_NAME} ${ARGN})
  ELSE(SWIG_MODULE_${name}_REAL_NAME)
    MESSAGE(SEND_ERROR "Cannot find Swig library \"${name}\".")
  ENDIF(SWIG_MODULE_${name}_REAL_NAME)
ENDMACRO(SWIG_LINK_LIBRARIES name)


# ---------------------------------------------------------------------------
#
# returns the leftmost portion of the path common to both files
#
function ( swig_find_common_path var file1 file2 )
#  message ( STATUS "IN: file1=${file1}  file2=${file2}" )
  get_filename_component ( f1r ${file1} REALPATH )
  get_filename_component ( f2r ${file2} ABSOLUTE )
#  message ( STATUS "OUT: file1=${f1r}  file2=${f2r}" )
  swig_get_path_as_list ( path1 ${f1r} )
#  message ( STATUS "path1=${path1}" )
  swig_get_path_as_list ( path2 ${f2r} )
#  message ( STATUS "path2=${path2}" )

  list ( LENGTH path1 l1 )
  list ( LENGTH path2 l2 )
  swig_max ( size ${l1} ${l2} )
#  message ( STATUS "size=${size} l1=${l1} l2=${l2}" )

  set ( com )
  while ( size GREATER 0 )
    list ( GET path1 0 name1 )
    list ( GET path2 0 name2 )
#    message ( STATUS "com=${com}  name1=${name1}  name2=${name2}" )
    if ( name1 STREQUAL name2 )
      list ( APPEND com ${name1} )
      list ( REMOVE_AT path1 0 )
      list ( REMOVE_AT path2 0 )
    else ()
      break ()
    endif ()
    math ( EXPR size "${size} - 1" ) # decrement size
  endwhile ()

  # revert the list to a path
  list ( LENGTH com len )
  if ( len GREATER 0 )
    list ( REVERSE com )
    list ( GET com 0 last )
    string ( REGEX MATCH "^.*${last}" com ${file1} )
    set ( ${var} ${com} PARENT_SCOPE )
  else ()
    set ( ${var} ${com} PARENT_SCOPE )
  endif ()

endfunction ()



# ---------------------------------------------------------------------------
#
# parses a path and puts its dirs a list left-to-right
#
function ( swig_get_path_as_list listvar file )
  get_filename_component ( path ${file} REALPATH )
  get_filename_component ( name ${path} NAME )
  get_filename_component ( path ${path} PATH )
  set ( lv "${name}" )
  get_filename_component ( lastName ${path} NAME )
#  message ( STATUS "${path} -------- ${name},${lastName} ---------- ${lv}" )
  while  ( NOT "${name}" STREQUAL "${lastName}")
    get_filename_component ( name ${path} NAME )
    list ( APPEND lv "${name}" )
    get_filename_component ( path ${path} PATH )
    get_filename_component ( lastName ${path} NAME )
#    message ( STATUS "${path} -------- ${name},${lastName} ---------- ${lv}" )
  endwhile ()
  list ( APPEND lv "${path}" )
  list ( REVERSE lv )
  set ( ${listvar} ${lv} PARENT_SCOPE )
endfunction ()



# ---------------------------------------------------------------------------
#
# returns the max of two args
#
function ( swig_max maxvar v1 v2 )
  if ( v1 GREATER v2 OR v1 EQUAL v2 )
    set ( m ${v1} )
  else ()
    set ( m ${v2})
  endif ()
  set ( ${maxvar} ${m} PARENT_SCOPE )
endfunction ()

