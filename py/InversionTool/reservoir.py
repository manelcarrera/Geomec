import logging
import helper #handle_exception
import numpy as np

class Reservoir:

	class Pos:
		Points	= 0
		Elems	= 1
		Params	= 2	# Thickness / Poisson's ratio Young's modulus / Pore pressure

	@staticmethod
	def get( filename ):
		try:
			from geomec_interface_ext import GeomecInterfaceExt as gm_ext
			return gm_ext.func( ('IO_gocad','readdata',filename), ) # caution with '\t' from '\top_reservoir.ts'
		except:
			helper.handle_exception()

	@staticmethod
	def transform( reservoir ):
		try:
			points	= reservoir[ Reservoir.Pos.Points ]
			params	= reservoir[ Reservoir.Pos.Params ] # -> 1D..4D : Thickness / Poisson's ratio Young's modulus / Pore pressure -> not in this order

			coors=[]
			pars=[]

			for i in range( len(points) ):

				coor=np.array( points[ i ] )
				coors.append( coor.astype(np.float))

				val=np.array( params[ i ] )
				pars.append(val.astype(np.float))

			_points = np.array(coors)
			_pars	= np.array(pars)

			return (_points,_pars)
			
		except:
			helper.handle_exception()

	@staticmethod
	def print_sizes(reservoir):

		points,elems,_=reservoir
		logging.debug( 'res : 4x : points_v, tri_v, params_v [4D]' )
		logging.debug( 'size: points:{} tri:{}'.format( len( points ), len( elems ) ) )


	@staticmethod
	def print_02(reservoir):

		try:
			points,elems,params=reservoir
			i=1
			for point in points:
				logging.debug( 'PVRTX {} {} {}'.format(i,point,params[0][i-1]) )
				i+=1
			for elem in elems:
				logging.debug( 'ELEM {}'.format( elem ) )
		except:
			helper.handle_exception()

	@staticmethod
	def print_(reservoir):

		#points,elems,params,header=reservoir
		points,elems,params=reservoir

		logging.debug( 'points: {}\n {}'.format( len(points), points ) )
		logging.debug( 'elems: {}\n {}'.format( len(elems), elems ) )
		logging.debug( 'params: {}\n {}'.format( len(params), params ) )
		#logging.debug( 'header: {}\n {}'.format( len(header), header ) )
