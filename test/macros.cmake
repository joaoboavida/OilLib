
#---------------------------------------------------------------------
# Defines a macro to create a target in a test
macro ( genexec SourceName targetName outputName )

    file ( GLOB hdrs "*.h" "*.hpp" )
#message( STATUS ${hdrs} )
 
    add_executable ( ${targetName} EXCLUDE_FROM_ALL ${SourceName} ${hdrs} )

    if ( TEST_DEPENDENCIES )
        add_dependencies( ${targetName} ${TEST_DEPENDENCIES} )
    endif ()
    target_link_libraries ( ${targetName} ${TEST_LIBRARIES} )
    
    add_dependencies ( ALL_TESTS ${targetName} )

    # create the executable in the test directory
    set_target_properties( ${targetName} PROPERTIES
                           OUTPUT_NAME "${outputName}"
                           # to organize targets in an IDE - not working in VS10 - not sure why
                           FOLDER "test"
                           RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )

endmacro () 


#---------------------------------------------------------------------
# Defines a macro to create tests, optionally with a multithreaded version. 
# Example usage:
#    generate_testexec ( main.cpp executable )        # single-threaded only
#    generate_testexec ( main.cpp executable ON )     # single- and multi-threaded
macro ( GENERATE_TESTEXEC SourceName ExecName )

    set ( targetName "test_${ExecName}" )
    set ( outputName "test_${ExecName}" )
    
    # generate the single threaded version
    genexec ( ${SourceName} ${targetName} ${outputName} )

    # and the multithreaded version if it is wanted 
    set ( MultiThreaded "${ARGV2}" )
    if ( MultiThreaded )
        if ( OPENMP_FOUND )
            
            set ( targetName "${targetName}_mt" )
            set ( outputName "${outputName}_mt" )
    
            #message ( STATUS "${ExecName}: creating also a multithreaded version: ${name}")
            genexec ( ${SourceName} ${targetName} ${outputName} )
            
            set_target_properties ( ${name} PROPERTIES
                                    COMPILE_FLAGS "${OpenMP_CXX_FLAGS} -DHAVE_OPENMP"
                                    LINK_FLAGS "${OpenMP_CXX_FLAGS}")
        
#        else ()
#            message ( WARNING "${ExecName}: multithreading asked but OpenMP was not found")
        endif ()
    endif ()

endmacro ( GENERATE_TESTEXEC )


# END

