#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------

dia_target( lb fw 51 ModelFrame )

set( public_include_dirs
  include
)

set ( needs 
  # add direct dependencies here
	${LBFWGE_L}
  ${LBGCCLASSES}
)

dia_add_sources(
  include
  Base
  Factory
)

add_definitions( -Dlbfw51ModelObjectFactory_EXPORTS )

set( extra_libraries ${FEMSYS_LIBS_NOGUI} ${QT_LIBRARIES} )

#------------------------------------------------------------------------------
dia_build_library( SHARED )
