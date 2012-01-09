# ########## OilLib Library Project ##########
cmake_minimum_required ( VERSION 2.6 )
cmake_policy ( SET CMP0015 OLD ) # link_directories() relative path handling
set_property ( GLOBAL PROPERTY USE_FOLDERS ON )

project ( OilLib )

set ( OILLIB_MAJOR_VERSION 0 CACHE STRING "OilLib Library major version number." FORCE )
set ( OILLIB_MINOR_VERSION 0 CACHE STRING "OilLib Library minor version number." FORCE )
set ( OILLIB_PATCH_VERSION 1 CACHE STRING "OilLib Library patch version number." FORCE )
set ( OILLIB_VERSION_STRING "${OILLIB_MAJOR_VERSION}.${OILLIB_MINOR_VERSION}.${OILLIB_PATCH_VERSION}" )


# ######### General setup ##########

option ( BUILD_SHARED_LIBS "Build shared libraries." OFF )
option ( CREATE_PYTHON_API_TARGET "Whether to provide a target for generating the Python API" ON )
#option ( BUILD_TESTS "Whether tests should be included in the makefile." ON )
#option ( BUILD_EXAMPLES "Whether examples should be included in the makefile." ON )

get_filename_component ( PROJECT_TOP_DIR "${PROJECT_SOURCE_DIR}" ABSOLUTE )

set ( OILLIB_INC_DIR "${PROJECT_TOP_DIR}/src" )
set ( OILLIB_SRC_DIR "${PROJECT_TOP_DIR}/src" )
set ( OILLIB_BIN_DIR "${PROJECT_TOP_DIR}/bin" )
set ( OILLIB_LIB_DIR "${PROJECT_TOP_DIR}/lib" )
set ( OILLIB_DOC_DIR "${PROJECT_TOP_DIR}/doc" )

set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OILLIB_LIB_DIR} CACHE PATH "Directory where static and import libraries are created." )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OILLIB_LIB_DIR} CACHE PATH "Directory where dynamic libraries and modules are created." )
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OILLIB_BIN_DIR} CACHE PATH "Directory where dynamic libraries and executables are created." )

set ( OILLIB_INCLUDE_DIRS ${OILLIB_INC_DIR} )
set ( OILLIB_LIBRARY_DIRS ${OILLIB_LIB_DIR} )
set ( OILLIB_LIBRARIES "" )


# ######### Process  ##########

if ( BUILD_SHARED_LIBS )
    set ( OILLIB_SHARED_BUILD 1 )
else ()
    unset ( OILLIB_SHARED_BUILD )
endif ()


# ######### Gather all the sources  ##########

configure_file ( ${OILLIB_INC_DIR}/config.hpp.in ${OILLIB_INC_DIR}/config.hpp )

set ( OILLIB_SOURCES 
    ${OILLIB_SRC_DIR}/hello.cpp
)
set ( OILLIB_PUBLIC_HEADERS
    ${OILLIB_INC_DIR}/config.hpp
    ${OILLIB_INC_DIR}/hello.hpp
)
set ( OILLIB_PRIVATE_HEADERS 
)
set ( OILLIB_HEADERS ${OILLIB_PUBLIC_HEADERS} ${OILLIB_PRIVATE_HEADERS} )
set ( OILLIB_ALL_SOURCES ${OILLIB_HEADERS} ${OILLIB_SOURCES} )

# interface for SWIG
set ( OILLIB_INTERFACE_FILES
    ${OILLIB_INC_DIR}/hello.swg
)

# ######### Create the library  ##########

include_directories ( ${OILLIB_INCLUDE_DIRS} )

add_library ( OilLib ${OILLIB_SOURCES} ${OILLIB_HEADERS} )
set_target_properties ( OilLib PROPERTIES
#   PROJECT_LABEL "Oil Library"                  # MCB: this affects dll name on WIN32
   VERSION ${OILLIB_VERSION_STRING}
   DEFINE_SYMBOL OILLIB_EXPORTS                  # for dllexport/dllimport decorations
   PUBLIC_HEADER "${OILLIB_PUBLIC_HEADERS}"
   OUTPUT_NAME "oil"
)


# ######### Create the API  ##########

set ( MODULE_NAME oil )

find_package ( SWIG REQUIRED )
include ( ${SWIG_USE_FILE} )

set ( CMAKE_SWIG_FLAGS "-c++" )

# see http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_CMake_to_generate_SWIG_wrapper_libraries.3F
set_source_files_properties ( ${OILLIB_INTERFACE_FILES} PROPERTIES CPLUSPLUS ON )
set_source_files_properties ( ${OILLIB_INTERFACE_FILES} PROPERTIES CMAKE_SWIG_FLAGS "-c++" )


# Python -------------------------

if ( CREATE_PYTHON_API_TARGET )

  find_package ( PythonLibs REQUIRED )

  set ( apiName "${MODULE_NAME}_python" )

  include_directories ( ${PYTHON_INCLUDE_PATH} )
  swig_add_module ( ${apiName} python ${OILLIB_INTERFACE_FILES} ${OILLIB_SOURCES} )
  swig_link_libraries ( ${apiName} ${PYTHON_LIBRARIES} )

endif ( CREATE_PYTHON_API_TARGET )


