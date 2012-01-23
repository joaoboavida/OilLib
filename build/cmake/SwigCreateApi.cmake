
# ######### Utilities to create an API using SWIG  ##########

find_package ( SWIG REQUIRED )
include ( UseSWIG.cmake )

option ( BUILD_API_ADD_TO_ALL "Whether the API should be built by default" OFF )

option ( BUILD_API_CSHARP "Whether to provide a target for generating the C# API" ON )
option ( BUILD_API_PHP5 "Whether to provide a target for generating the PHP5 API" ON )
option ( BUILD_API_PYTHON "Whether to provide a target for generating the Python API" ON )
option ( BUILD_API_RUBY "Whether to provide a target for generating the Ruby API" ON )

include ( CMakeParseArguments )

#===========================================

#
# SWIG_CREATE_API ( NAME name
#                   OUTDIR dir
#                   INTERFACE_FILES file1.swg file2.swg ...
#                   SOURCE_FILES file1.hpp file2.cpp ...
#                   ADDITIONAL_LIBS lib1target lib2target
#                 )
#
macro ( swig_create_api )

  # ------------------------------------------------------------------
  # parse arguments
  #usage of cmake_parse_arguments: http://www.cmake.org/Wiki/CMakeMacroParseArguments
  set ( options )
  set ( oneValueArgs NAME OUTDIR )
  set ( multiValueArgs INTERFACE_FILES SOURCE_FILES ADDITIONAL_LIBS )
  cmake_parse_arguments ( SWGCA "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  #message ( STATUS "NAME=${SWGCA_NAME}" )
  #message ( STATUS "OUTDIR=${SWGCA_OUTDIR}" )
  #message ( STATUS "INTERFACE_FILES=${SWGCA_INTERFACE_FILES}" )
  #message ( STATUS "SOURCE_FILES=${SWGCA_SOURCE_FILES}" )
  #message ( STATUS "ADDITIONAL_LIBS=${SWGCA_ADDITIONAL_LIBS}" )

  # General setup -------------------------
  add_custom_target ( API COMMENT "Building API." )
  include_directories ( "${SWGCA_OUTDIR}" )  # write .py, .php, etc in the outdir
  # see http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_CMake_to_generate_SWIG_wrapper_libraries.3F
  set_source_files_properties ( ${SWGCA_INTERFACE_FILES} PROPERTIES CPLUSPLUS ON )
  if ( API_ADD_TO_ALL )
    add_dependencies ( all API )
  endif ()

  # many of the cmake find files were taken from gdcm:
  # http://sourceforge.net/projects/gdcm/

  # Csharp -------------------------
  if ( BUILD_API_CSHARP )
    find_package ( CSharp REQUIRED )
    set ( apiName "api_csharp" )
    set ( CMAKE_SWIG_OUTDIR "${SWGCA_OUTDIR}/csharp" )
    file ( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
    swig_add_module ( ${apiName} CSharp ${SWGCA_INTERFACE_FILES} ${SWGCA_SOURCE_FILES} )
    # not necessary to add_incs_to_target()
    set_target_properties ( ${apiName} PROPERTIES
      EXCLUDE_FROM_ALL 1
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      OUTPUT_NAME ${SWGCA_NAME}
      COMMENT "API was put into ${CMAKE_SWIG_OUTDIR}"
    )
    target_link_libraries ( ${apiName} ${SWGCA_ADDITIONAL_LIBS} )
    add_dependencies ( API ${apiName} )
  endif ()

  # PHP5 -------------------------
  if ( BUILD_API_PHP5 )
    find_package ( PHP5 REQUIRED )
    set ( apiName "api_php5" )
    set ( CMAKE_SWIG_OUTDIR "${SWGCA_OUTDIR}/php5" )
    file ( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
    swig_add_module ( ${apiName} php ${SWGCA_INTERFACE_FILES} ${SWGCA_SOURCE_FILES} )
    add_incs_to_target ( ${apiName} ${PHP5_INCLUDE_PATH} ${CMAKE_SWIG_OUTDIR} )
    set_target_properties ( ${apiName} PROPERTIES
      EXCLUDE_FROM_ALL 1
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      OUTPUT_NAME ${SWGCA_NAME}
      COMMENT "API was put into ${CMAKE_SWIG_OUTDIR}"
    )
    target_link_libraries ( ${apiName} ${SWGCA_ADDITIONAL_LIBS} )
    add_dependencies ( API ${apiName} )
  endif ()


  # Python -------------------------
  if ( BUILD_API_PYTHON )
    find_package ( PythonLibs REQUIRED )
    set ( apiName "api_python" )
    set ( CMAKE_SWIG_OUTDIR "${SWGCA_OUTDIR}/python" )
    file ( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
    swig_add_module ( ${apiName} python ${SWGCA_INTERFACE_FILES} ${SWGCA_SOURCE_FILES} )
    swig_link_libraries ( ${apiName} ${PYTHON_LIBRARIES} )
    set ( apiName "_${apiName}" )
    add_incs_to_target ( ${apiName} "${PYTHON_INCLUDE_PATH}" )
    set_target_properties ( ${apiName} PROPERTIES
      EXCLUDE_FROM_ALL 1
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      OUTPUT_NAME ${SWGCA_NAME}
      PREFIX "_"  # name it as _extension.so (linux)
      COMMENT "API was put into ${CMAKE_SWIG_OUTDIR}"
    )
    target_link_libraries ( ${apiName} ${SWGCA_ADDITIONAL_LIBS} )
    add_dependencies ( API ${apiName} )
  endif ()


  # Ruby -------------------------
  if ( BUILD_API_RUBY )
    find_package ( Ruby REQUIRED )
    set ( apiName "api_ruby" )
    set ( CMAKE_SWIG_OUTDIR "${SWGCA_OUTDIR}/ruby" )
    file ( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
    swig_add_module ( ${apiName} ruby ${SWGCA_INTERFACE_FILES} ${SWGCA_SOURCE_FILES} )
    add_incs_to_target ( ${apiName} "${RUBY_INCLUDE_DIRS}" )
    set_target_properties ( ${apiName} PROPERTIES
      EXCLUDE_FROM_ALL 1
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
      OUTPUT_NAME ${SWGCA_NAME}
      PREFIX "" # name it as extension.so (linux)
      COMMENT "API was put into ${CMAKE_SWIG_OUTDIR}"
    )
    target_link_libraries ( ${apiName} ${SWGCA_ADDITIONAL_LIBS} )
    add_dependencies ( API ${apiName} )
  endif ()

endmacro ( swig_create_api )



#===========================================

#
# - add per target include dirs
#
function ( add_incs_to_target targetName )
  if ( UNIX )
    set ( iopt "-I" )
  elseif ( WIN32 )
    set ( iopt "/I" )
  endif ( UNIX )
  foreach ( i ${ARGV} )
    set ( iflags "${iflags} ${iopt}${i}" )
  endforeach ( i )
  set_target_properties ( ${targetName} PROPERTIES
    COMPILE_FLAGS "${iflags}"
  )
endfunction ( add_incs_to_target )

