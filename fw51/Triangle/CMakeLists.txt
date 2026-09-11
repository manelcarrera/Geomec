#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 Triangle )

set( public_include_dirs
  include
)

set( private_include_dirs
	Triangle
)

set ( needs 
	${LBFWGE_L}
)

dia_add_sources(
  Triangle
  include
  Base
)

#------------------------------------------------------------------------------
dia_build_library()
