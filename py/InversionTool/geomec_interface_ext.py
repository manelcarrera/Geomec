import sys
import helper #handle_exception
import logging

#from Tkinter import * # for tkinter siumulation

import geomec_ext

class GeomecInterfaceExt():

	class Request_Id:

		Cmd_Get_Reservoir_From_File		= 100
		Cmd_Get_Displacement_From_File	= 101
		Cmd_Get_Generic					= 102
		Cmd_None						= 103

		name = (
			'Cmd_Get_Reservoir_From_File',
			'Cmd_Get_Displacement_From_File',
			'Cmd_Get_Generic',
			'Cmd_None'
		)


	def __init__( self ):
		pass

	def print_func_name( self ):
		print('DECORATOR')

	#------------------------------------------------------------------------------------ set: TI -> GM
	#@print_func_name
	def set_num( self ):
		geomec_ext.set_num( 10 )

	def set_array( self ):
		pass
		'''
		import numpy as np
		a = np.arange( 15 ).reshape( 3, 5 )	
		b = 10*np.sin( a )
		print( a )
		print( b )
		geomec.set_array( b )
		'''

	#------------------------------------------------------------------------------------ get: TI <- GM
	def get_num( self ):
		print( geomec_ext.get_num() )

	def get_array( self ):
		#print( emb.numargs() )
		#print( emb.get_array( _b[ 0 ] ) )
		print( geomec_ext.get_array() )

	def get_cmd( self ):
		cmd = geomec_ext.get_cmd()
		print( cmd )

	@staticmethod
	def reservoir( filename ):
		return geomec_ext.reservoir( filename )
		#reservoirpoints, reservoirtriangles, reservoirpars, reservoirheader = geomec.get_reservoir(  filename )
		#print( reservoirpoints[ 0 ] )

	@staticmethod
	def func( params ):
		try:
			return geomec_ext.func( params )
		except:
			helper.handle_exception()

	@staticmethod
	def displacement( filename ):
		# file format: colons: x	y	z	dz
		#xirr, yirr, zirr, wdat, subs, header = geomec.displacement( filename )
		#print( xirr[ 0 ] )
		return geomec_ext.displacement( filename )

	#-------------------------- show gui: GM -> IT

	'''
	def gui_01( self ):
		root = Tk()
		w = Label(root, text="Hello, world!")
		w.pack()
		root.mainloop()

	def gui_02( self ):
		# GUI
		root = Tk()
		dlg = Dialog( root )

		# worker thread
		t = Thread( target = worker, args = ( dlg, ) )
		t.daemon = True
		t.start()

		root.mainloop()
		root.destroy() # optional; see description below
	'''

'''
def main( method_name ):

	print( method_name )

	gi = GeomecInterface()
	method = getattr( gi, method_name )
	method()
	
if __name__ == "__main__":
	main( sys.argv[ 1 ] )
'''
