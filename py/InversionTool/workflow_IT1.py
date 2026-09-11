import logging
import helper
from geomec_interface import GeomecInterface as GI # GI.Cmd
from workflow import Workflow
from reservoir import Reservoir
from displacement import Displacement

class Workflow_IT1( Workflow ):

	def __init__( self, gui ):
		Workflow.__init__( self, gui )

	def cmd( self, cmd_id ):
		
		logging.debug('**************** {} ****************'.format(GI.Cmd.name[cmd_id]))	
		cmd_=[]
		self.current_cmd = cmd_id
		try:
			cmd_ = [ self.commands.build( cmd_id ) ]
		except:
			helper.handle_exception()
		return cmd_

	def set_reservoir( self, res_list ):
		try:
			
			cmd_id,reservoir=res_list[0]

			self.gui.reservoirpoints,self.gui.reservoirtriangles,self.gui.reservoirpars=reservoir
			import numpy as np
			points=np.array(self.gui.reservoirpoints)
			x_min, y_min, z_min = points.min( axis=0 )
			x_max, y_max, z_max = points.max( axis=0 )
			logging.debug( 'x:{}-{} y:{}-{} z:{}-{}'.format( x_min, x_max, y_min, y_max, z_min, z_max ) )

			Reservoir.print_sizes( reservoir )
			self.gui.reservoir()
			
		except:
			helper.handle_exception()

	def set_displacement( self, res_list ):
		try:
			cmd_id,displacement=res_list[0]
			self.gui.elems, self.gui.zirr, self.gui.wdat, self.gui.subs = Displacement.transform( displacement )
			self.gui.subsidence(gm_results=True)
		except:
			helper.handle_exception()

	def set_measured_displacement( self, res_list ):
		try:
			# KB Geomec supplies N E z
			cmd_id,displacement=res_list[0]
			self.gui.elems, self.gui.zirr, self.gui.wdat, self.gui.subs = Displacement.transform( displacement )
			self.gui.subsidence()
		except:
			helper.handle_exception()

	def handle_primitives( self, res_list ):
		try:
			cmd_id,res=res_list[0]
		except:
			helper.handle_exception()

	'''
	def set_material_parameter( self, val ):
		#self.gui.poisson_ratio_val=val #TODO:
		pass
	def set_depletion( self, val ):
		pass
	'''


