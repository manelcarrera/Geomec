#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8.6 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 OpenGLFrame )

set( public_include_dirs
  include
)

set ( needs
  # add direct dependencies here
  ${LBFWGE_L}
)

dia_add_sources(
  include
  Base
)

set( extra_libraries ${QT_LIBRARIES} ${OPENGL_LIBRARIES} )

#------------------------------------------------------------------------------
dia_build_library()
