#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 Tensor )

set( public_include_dirs
  include
)

set ( needs 
	${LBFWGE_L}
)

dia_add_sources(
	Base
)

#------------------------------------------------------------------------------
dia_build_library()

