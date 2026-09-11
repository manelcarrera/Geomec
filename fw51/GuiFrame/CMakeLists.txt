#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 GuiFrame )

set( public_include_dirs
  include
)

set ( needs 
	${LBFWMF_L}
	${LBFWOG_L}
	${LBFWGE_L}
)

dia_add_sources(
  include
  Base
)

set( extra_libraries ${QT_LIBRARIES} )

#------------------------------------------------------------------------------
dia_build_library()
