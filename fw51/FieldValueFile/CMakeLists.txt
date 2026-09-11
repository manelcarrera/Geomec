#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 FieldValueFile )

set( public_include_dirs
  include
)

set ( needs 
	${LBFWGE_L}
	${LBUI}
)

dia_add_sources(
  include
  Base
)

set( extra_libraries ${QT_LIBRARIES} )

#------------------------------------------------------------------------------
dia_build_library()
