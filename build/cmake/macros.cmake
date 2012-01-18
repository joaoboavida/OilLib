#######################################


#
# - per target include dirs
# - BEWARE: when calling with incs as a list use quotes: eg
#           add_incs_to_target ( targetName "${list_of_include_paths}" )
#
function ( add_incs_to_target targetName incs )
  if ( UNIX )
    set ( iopt "-I" )
  elseif ( WIN32 )
    set ( iopt "/I" )
  endif ( UNIX )
  foreach ( i ${incs} )
    set ( iflags "${iflags} ${iopt}${i}" )
  endforeach ( i )
  set_target_properties ( ${targetName} PROPERTIES
    COMPILE_FLAGS "${iflags}"
  )
endfunction ( add_incs_to_target )


