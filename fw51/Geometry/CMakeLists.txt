#------------------------------------------------------------------------------
# vim :set ft=cmake:
cmake_minimum_required( VERSION 2.8 )
find_package( DianaBuildSystem HINTS $ENV{DIA_CMAKE_ROOT} )
#------------------------------------------------------------------------------
dia_target( lb fw 51 Geometry )

set( needs ${LBEL} ${LBCX} )

set( public_include_dirs
  include
)

dia_add_sources(
  include    Base    Coordinate Dispatch
  Element    Matrix  Mesh       NearestNeighbour
  Polyhedron Surface Volume
)

#------------------------------------------------------------------------------
dia_build_library()
