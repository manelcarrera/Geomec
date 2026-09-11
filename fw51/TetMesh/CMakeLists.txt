#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 TetMesh )

set( public_include_dirs
  include
	tet_src
)

set ( needs 
	${LBFWGE_L}
	${LBUI}
	${LBGS}
)

dia_add_sources(
  include
  tet_src
  Base
)

#------------------------------------------------------------------------------
dia_build_library()
