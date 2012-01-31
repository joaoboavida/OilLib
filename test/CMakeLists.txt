# Make sure the compiler can find include files from our DEB library.
include_directories ( ${OILLIB_INCLUDE_DIRS} )

# Make sure the linker can find the DEB library once it is built.
link_directories ( ${OILLIB_LIBRARY_DIRS} )

# Add here dependencies & libraries necessary for all tests
set ( TEST_DEPENDENCIES DEB )
set ( TEST_LIBRARIES ${OILLIB_LIBRARIES} ) 
if ( WIN32 )
    list ( APPEND TEST_LIBRARIES winmm )
endif ()

find_package ( OpenMP )
include ( macros.cmake )

# Add our target group
add_custom_target ( ALL_TESTS COMMENT "Building Tests." )
set_target_properties ( ALL_TESTS PROPERTIES
                        # to organize targets in an IDE
                        FOLDER "test" )

add_subdirectory ( 000 )

# END

