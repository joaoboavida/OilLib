# ######### Create the API  ##########


option ( BUILD_API_ADD_TO_ALL "Whether the API should be built by default" OFF )
option ( BUILD_API_CSHARP "Whether to provide a target for generating the C# API" ON )
option ( BUILD_API_PHP5 "Whether to provide a target for generating the PHP5 API" ON )
option ( BUILD_API_PYTHON "Whether to provide a target for generating the Python API" ON )


# General setup -------------------------

add_custom_target ( API COMMENT "Building API." )

set ( MODULE_NAME oillib )

find_package ( SWIG REQUIRED )
include ( UseSWIG.cmake )

set ( CMAKE_SWIG_FLAGS "-c++" )
# write .py, .php, etc in the lib dir
set ( OILLIB_SWIG_OUTDIR "${OILLIB_LIB_DIR}" )
include_directories( "${OILLIB_LIB_DIR}" )

# see http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_CMake_to_generate_SWIG_wrapper_libraries.3F
set_source_files_properties ( ${OILLIB_INTERFACE_FILES} PROPERTIES CPLUSPLUS ON )
set_source_files_properties ( ${OILLIB_INTERFACE_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-c++" )

if ( API_ADD_TO_ALL )
  add_dependencies ( all API )
endif ()


# Csharp -------------------------

if ( BUILD_API_CSHARP )
  find_package ( CSharp REQUIRED )
  set ( apiName "${MODULE_NAME}_csharp" )
  set ( CMAKE_SWIG_OUTDIR "${OILLIB_SWIG_OUTDIR}/cs" )
  file( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
  swig_add_module ( ${apiName} CSharp ${OILLIB_INTERFACE_FILES} ${OILLIB_SOURCE_FILES} )
  # not necessary to add_incs_to_target()
  set_target_properties ( ${apiName} PROPERTIES
    EXCLUDE_FROM_ALL 1
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
  )
  add_dependencies ( API ${apiName} )
endif ()


# PHP5 -------------------------

if ( BUILD_API_PHP5 )
  find_package ( PHP5 REQUIRED )
  set ( apiName "${MODULE_NAME}_php" )
  set ( CMAKE_SWIG_OUTDIR "${OILLIB_SWIG_OUTDIR}/php5" )
  file( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
  swig_add_module ( ${apiName} php ${OILLIB_INTERFACE_FILES} ${OILLIB_SOURCE_FILES} )
  add_incs_to_target ( ${apiName} ${PHP5_INCLUDE_PATH} ${CMAKE_SWIG_OUTDIR} )
  set_target_properties ( ${apiName} PROPERTIES
    EXCLUDE_FROM_ALL 1
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
  )
  add_dependencies ( API ${apiName} )
endif ()


# Python -------------------------

if ( BUILD_API_PYTHON )
  find_package ( PythonLibs REQUIRED )
  set ( apiName "${MODULE_NAME}_python" )
  set ( CMAKE_SWIG_OUTDIR "${OILLIB_SWIG_OUTDIR}/python" )
  file( MAKE_DIRECTORY ${CMAKE_SWIG_OUTDIR} )
  swig_add_module ( ${apiName} python ${OILLIB_INTERFACE_FILES} )
  swig_link_libraries ( ${apiName} ${PYTHON_LIBRARIES} )
  set ( apiName "_${apiName}" )
  add_incs_to_target ( ${apiName} "${PYTHON_INCLUDE_PATH}" )
  set_target_properties ( ${apiName} PROPERTIES
    EXCLUDE_FROM_ALL 1
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SWIG_OUTDIR}"
  )
  add_dependencies ( API ${apiName} )
endif ()
