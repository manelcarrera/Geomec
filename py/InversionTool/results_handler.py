import logging

from geomec_interface import GeomecInterface as GI # GeomecInterface.Request_Id 
from geomec_interface_ext import GeomecInterfaceExt # GeomecInterfaceExt.Request_Id 

import helper #handle_exception

from reservoir import Reservoir
from displacement import Displacement

from workflow import Workflow_IT1, Workflow_IT2

class ResultsHandler():
	'''
	Handles the responses to a requests sent to Geomec.
	It keeps a pointer to the GUI.
	'''

	class Constants:
		LIST_DELIMITER = '*'

	def __init__( self, gui, iface ):
		self.gui = gui
		self.iface = iface

	def handle( self, res ):

		try:
			logging.debug('   <--')
			logging.debug('handle:{}'.format( res ))

			'''
			if isinstance( self.iface.workflow, Workflow_IT1 ):
				self.handle_cmd_IT1( res[ 0 ]) 
			else:
				self.iface.workflow.on_result( res ) 
			'''
			self.iface.workflow.on_result( res ) 
				
		except:
			helper.handle_exception()

	'''
	def set_reservoir_list( self, res ):				self.fill_combo(res,self.gui.cb_res)
	def set_displacement_list( self, res ):				self.fill_combo(res,self.gui.cb_dis)
	def set_measured_displacement_list( self, res ):	self.fill_combo(res,self.gui.cb_dis_meas)
	def set_depletion_list( self, res ):				self.fill_combo(res,self.gui.cb_dep)
	def set_surface_list( self, res ):					self.fill_combo(res,self.gui.cb_sur)
	'''

	#-----------------------------------------------------------------------------------

def test_1():
	logging.basicConfig(	filename="c:\Temp\IT_py.log", 
							format='%(asctime)s %(message)s',
							level=logging.DEBUG)


	logging.debug("MCR")

	res=( 1,('Hola*Adios',) )
	cmd_id,params=res

	#logging.debug( res )

	#res = res[1:]

	logging.debug( 'res:{} params:{}'.format(res,params) )

	handler = ResultsHandler( None )

	handler.set_reservoir_list( params )

#test_1()

'''
import numpy as np
a = [ [ -708.97241211, -1801.56030273,   999.99993896 ] ]
logging.debug('a:{}'.format( a ) )

b = np.array( a )
logging.debug('b:{}'.format( b ) )

#a = [ [ 1, 2, 3 ] ]

# [ -662.73815918 -1835.17626953  1000.        ]
# [ -662.18603516 -1793.53405762  1000.        ] ]
'''


'''
import numpy as np

a0 = ['x','y','z']

a = np.array( a0 )
logging.debug('a:{}'.format( a ) )

b = a[:,0:2]
logging.debug('b:{}'.format( b ) )
'''

		