import logging 
import helper #handle_exception
import numpy as np

class Displacement:

	class Pos:
		elem=0
		Wdat=1
		Subs=2
		#Header=3

	@staticmethod
	def get( filename ):
		try:
			from geomec_interface_ext import GeomecInterfaceExt as gm_ext
			return gm_ext.func( ('IO_gocad','getmeasure',filename ) )
			#logging.debug( 'reservoir : x:{} y:{} z:{} wdat:{} subs:{} header:{}'.format( x, y, z, wdat, subs, header ) )
		except:
			helper.handle_exception()

	@staticmethod
	def transform( displacement ):
		try:
			#_elems,_z,_wdat,_subs,_header=displacement
			_elems,_z,_wdat,_subs=displacement

			subs=[]
			wdat=[]

			for __subs in _subs:
				val=np.array( __subs )
				subs.append( val.astype(np.float) )
				wdat.append( _wdat )


			elems_		= np.array( _elems )
			z_ = np.array( _z )
			wdat_	= np.array( wdat )
			subs_	= np.array( subs )

			#header_	= _header.strip().split() # must be a list even if it has just one element 

			#return (elems_,z_,wdat_,subs_,header_)
			return (elems_,z_,wdat_,subs_)

		except:
			helper.handle_exception()

	@staticmethod
	def print_sizes(displacement):

		x,_,_,_,subs=displacement
		logging.debug( 'res : 6x : x_v, y_v, z_v, wdat, subs[n:1..3]' )
		logging.debug( 'size: x,y,z:{} subs:{}'.format( len( x ), len( subs) ) )

	@staticmethod
	def print_02(displacement): #just 'dz'

		x,y,z,_,subs=displacement
		logging.debug( 'size: x:{} y:{} z:{} subs:{}'.format( len( x ), len( y ), len( z ), len( subs ) ) )
		'''
		i=0
		for subs_ in subs:
			logging.debug( 'DISPL {} {} {} {} - {} {} {}'.format( i, x[i], y[i], z[i], subs[i][0], subs[i][1], subs[i][2] ) )
			logging.debug( 'DISPL {} {} {} {} - {} {}'.format( i, x[i], y[i], z[i], subs[i][0], subs[i][1] ) )
			logging.debug( 'DISPL {} {} {} {} - {}'.format( i, x[i], y[i], z[i], subs[i][0] ) )
			i+=1
		'''

	@staticmethod
	def print_(displacement):

		elems,wdat,subs=displacement

		logging.debug( 'elems: {}\n {}'.format( len(elems), elems ) )
		logging.debug( 'wdat:{}'.format(wdat) )
		logging.debug( 'subs: {}\n {}'.format( len(subs), subs ) )

	@staticmethod
	def printKB_(displacement):

		test=displacement

		logging.debug( 'len(displacement): {}\n {}'.format( len(test), test ) )

