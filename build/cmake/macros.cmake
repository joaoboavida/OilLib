

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

