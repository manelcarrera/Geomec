import logging
import helper
from geomec_interface import GeomecInterface as GI # GI.Cmd
import numpy as np
from workflow import Workflow
from interface import Controls
from settings1 import settings1
#from control import Sender

class Workflow_IT2( Workflow ):

	def __init__( self, gui ):
		try:
			Workflow.__init__( self, gui )
			self.controls = Controls( gui )
			self.pointset_created = False; # used at pressure add; a pointset needs to be created but just first time; then values are just added to this pointset (so do not crete it each time vales are added)
			self.subs=[] #FIXME
		except:
			helper.handle_exception()

	def params( self, cmd_id, params_dict ):

		# auto:		command params and values got from controls (so they are not specified)
		# non-auto:	command params and values must be specified (gives some flexibility)

		# all params: 'list','res','sur', 'composite_idx','ps_id','dep','composite', 'component','depletion_type','output','prop_form','prop_mat','prop_dep'

		try:
			auto = True if not len( params_dict ) else False

			if auto:
				# from GUI controls
				params_dict['res']				= Controls.value_cb( self.gui.combos['res'] )
				params_dict['sur']				= Controls.value_cb( self.gui.combos['sur'] )
				params_dict['composite_idx']	= Controls.value_cb( self.gui.combos['composite_idx'] )
				#params_dict['dep_type']		= Controls.value_rb( self.gui.radios['dep_type'] )
				params_dict['dep_type']			= Controls.value_cb( self.gui.combos['dep_type'] )
				#params_dict['dep_action']		= Controls.value_rb( self.gui.radios['dep_action'] )
				params_dict['dep_action']		= Controls.value_cb( self.gui.combos['dep_action'] )
				# hard-coded
				params_dict['component']		= settings1['component']
				params_dict['composite']		= settings1['composite']
				params_dict['dep']				= settings1['dep']
				params_dict['dep_output']		= settings1['dep_output']

				params_dict['prop_form']		= settings1['prop_form']
				params_dict['prop_mat']			= settings1['prop_mat']
				params_dict['prop_dep']			= settings1['prop_dep']
				params_dict['ps_id']			= settings1['ps_id']
				params_dict['list']				= settings1['list']
				params_dict['mat_idx']			= settings1['mat_idx']
				params_dict['mat_type']			= settings1['mat_type']
				params_dict['param_idx']		= settings1['param_idx']
				params_dict['param_value']		= settings1['param_value']

				if cmd_id == GI.Cmd.IT1.Set.Depletion and params_dict['dep_action'] == GI.Depletion.Action.Replace:
					params_dict['composite_idx'] = settings1['composite_idx']

			# this filter should ebe done just for 'auto'
			params_dict_cmd={}
			for param in GI.Cmd.params[ cmd_id ]:
				if param in params_dict:
					params_dict_cmd[ param ]=params_dict[ param ]
				else:
					logging.error('cmd: {} : param: {} : NEEDED BUT NOT PROVIDED'.format( GI.Cmd.name[ cmd_id ], param ))
			
			return params_dict_cmd
		except:
			helper.handle_exception()

	def cmd( self, cmd_id, params_dict ):
		# auto:		command params and values got from controls (so they are not specified)
		# non-auto:	command params and values must be specified (gives some flexibility)

		auto = True if not len( params_dict ) else False
		p = self.params( cmd_id, params_dict )
		#self.cmds_queue.params
		#cmd_ = self.cmd_gen( cmd_id, params_, auto )

		logging.info('**************** {} **************** (GEN)({})'.format(GI.Cmd.name[cmd_id], 'AUTO' if auto else 'NO auto'))	
		logging.info('params_dict: {}'.format( p ))	
		logging.debug('')
		cmd_=[]

		try:

			#Lists
			if		cmd_id == GI.Cmd.IT1.List.Reservoir:				cmd_ = [	( GI.Cmd.Gen.List,			(GI.List.Reservoir) ) ]										# ( type_ )
			elif	cmd_id == GI.Cmd.IT1.List.Surface:					cmd_ = [	( GI.Cmd.Gen.List,			(GI.List.Surface, p['res']) ) ]									# params: ( type_, param ) ... param: res
			elif	cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	cmd_ = [	( GI.Cmd.Gen.List,			(GI.List.DB_Composite, p['composite']) ) ]					# ( list type )
			#Get
			elif	cmd_id == GI.Cmd.IT1.Get.Reservoir:					cmd_ = [	( GI.Cmd.Model.Elems,		(p['res'],p['sur']) ),										# ( res, sur )
																					( GI.Cmd.Model.Points,		(p['res'],p['sur']) ),										# ( res, sur )
																					#( GI.Cmd.Model.Prop.Form,	(p['res'],p['sur'],p['dep'], GI.Prop.Form.Thickness) ),				# ( res, sur, dep, prop )
																					( GI.Cmd.Model.Prop.Form,	(p['res'],p['sur'], GI.Prop.Form.Thickness) ),						# ( res, sur, prop )
																					( GI.Cmd.Model.Prop.Mat,	(p['res'],p['sur'],p['dep'], GI.Prop.Mat.PoissonsRatio) ),			# ( res, sur, dep, prop )
																					( GI.Cmd.Model.Prop.Mat,	(p['res'],p['sur'],p['dep'], GI.Prop.Mat.YoungsModulus) ),			# ( res, sur, dep, prop )
																					( GI.Cmd.Model.Prop.Dep,	(p['res'],p['sur'],p['dep'], GI.Prop.Dep.PorePressure) ) ]			# ( res, sur, dep, prop ) )

			elif	cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:		cmd_ = [	( GI.Cmd.DB.Points,			(p['composite'], p['composite_idx'] ) ),						# ( composite, composite_idx )
																					( GI.Cmd.DB.Values,			(p['composite'], p['composite_idx'], p['component'] ) ) ]		# ( composite, composite_idx, component )

			elif	cmd_id == GI.Cmd.IT1.Get.Displacement:				cmd_ = [	( GI.Cmd.DB.Points,			(p['composite'], p['composite_idx']) ),					# ( composite, composite_idx )
																					( GI.Cmd.Res.Values,		(p['composite'], p['component'], p['composite_idx'],p['dep']) ) ]	# ( composite, component, composite_idx, dep )
			#Set
			elif	cmd_id == GI.Cmd.IT1.Set.Depletion: #FIXME: 'dep' case in not coonsidered; 'dep' used only when link
				from depletion import Depletion

				if p['dep_action'] == GI.Depletion.Action.New:
					cmd_.append(	( GI.Cmd.Model.Depletion.Add,	(p['dep_output']) ) )

				if not self.pointset_created:
					cmd_.append(	( GI.Cmd.DB.PointSet.Create,	(p['ps_id']),				Depletion.points( p['dep_type'], self.gui ) ) )	# ( id, depletion_type ) (points)
					self.pointset_created = True

				if p['dep_action'] == GI.Depletion.Action.Replace:
					cmd_.append(	( GI.Cmd.DB.PointSet.ReplaceValues,	(p['ps_id'], p['composite_idx']),	Depletion.values( p['dep_type'], self.gui ) ) )	# ( id ) (values) #FIXME: '2*' just for test
				else:
					cmd_.append(	( GI.Cmd.DB.PointSet.AddValues,	(p['ps_id'], p['dep_type']),	Depletion.values( p['dep_type'], self.gui ) ) )	# ( id ) (values)
					cmd_.append(	( GI.Cmd.DB.PointSet.Link,		(p['ps_id'], p['res'], p['dep']) ) )						# ( id, res, dep  ) 

			#default
			else:														cmd_ = [ self.commands.build( cmd_id, p ) ]
		except:
			helper.handle_exception()

		return cmd_,p


	def set_reservoir( self, res_list ):
		try:
			import numpy as np

			elems_,points_,th_,pr_,ym_,pp_ = res_list			
			elems=elems_[1]
			points=points_[1]
			self.gui.th=np.array( th_[1][0] )
			self.gui.pr=np.array( pr_[1][0] )
			self.gui.ym=np.array( ym_[1][0] )
			self.gui.pp=np.array( pp_[1][0] )		
			
			self.gui.reservoirpoints=points[0]
			self.gui.reservoirtriangles=elems[0]

			'''
			logging.debug('elems({}x):{} points({}x):{} th({}x):{} pr({}x):{} ym({}x):{} pp({}x):{}'.format(
				len(self.gui.reservoirtriangles),self.gui.reservoirtriangles[0:self.LINES],
				len(self.gui.reservoirpoints),self.gui.reservoirpoints[0:self.LINES],
				len(self.gui.th),self.gui.th[0:self.LINES],
				len(self.gui.pr),self.gui.pr[0:self.LINES],
				len(self.gui.ym),self.gui.ym[0:self.LINES],
				len(self.gui.pp),self.gui.pp[0:self.LINES] ) )	
			'''

			#self.gui.reservoir()
			#self.cmds_queue.cmds.insert(0, GI.Cmd.Operation.Reservoir)
			#self.current_cmd = GI.Cmd.NoCmd # signal to test process to pick up a new command from the comds queue

		except:
			helper.handle_exception()

	def set_displacement( self, res_list ):
		'''
		Modelled displacement from Geomec
		'''
		try:
			points_,subs_ = res_list
			#logging.debug('self.subs in set_displacement: {}'.format(subs_[1][0]))
			points=points_[1][0]
			subs=subs_[1][0] # Geomec returns array in tuple
			self.gui.subs = subs.flatten() # Geomec returns each value in a list of length 1 within an array
			#if isinstance(subs_[1], tuple): 
			#	subs = subs_[1][0] # Geomec returns array in Tuple
			#	subs = subs.flatten() # Geomec returns each value in a list of length 1 within an array
			#else: # no tuple, only an array to flatten
			#	subs = subs_[1].flatten()
			self.gui.usex = False
			self.gui.usey = False
			self.gui.usez = True
			self.gui.subs = np.array([np.full(len(self.gui.subs), np.nan), np.full(len(self.gui.subs), np.nan), self.gui.subs])
			self.gui.subs = self.gui.subs.transpose()
			  
			#self.gui.subs=subs # returns a tuple
			# DO NOT call 'self.gui.subsidence()' from here !!!!!!!!!!!
			#self.gui.subsidence()
	
			#logging.debug('modelled disp : points({}x):{} subs({}x):{}'.format(len(points),points[0:self.LINES],len(subs),subs[0:self.LINES]))				
			logging.debug('modelled disp : {}, self.gui.subs: {}'.format(len(self.gui.subs), self.gui.subs))			
		except:
			helper.handle_exception()

	def set_measured_displacement( self, res_list ):
		try:
			points_,subs_ = res_list
			points=points_[1][0]
			subs=subs_[1][0]
			# KB: Normal people use [x, y, z], GEOMEC uses Y, X, Z!!!!!            
			self.gui.xirr = points[:,1]
			self.gui.yirr = points[:,0]
			self.gui.zirr = points[:,2]
			self.gui.subs = subs.flatten()
			# KB: Enforce that self.subs is always a 3-column np array [dx, dy, dz]
			self.gui.usex = False
			self.gui.usey = False
			self.gui.usez = True
			self.gui.subs = np.array([np.full(len(self.gui.subs), np.nan), np.full(len(self.gui.subs), np.nan), self.gui.subs])
			self.gui.subs = self.gui.subs.transpose()
			logging.debug("self.gui.subs in set_measured_displacement: {}".format(self.gui.subs))
			# DO NOT call 'self.gui.subsidence()' from here !!!!!!!!!!!!!

			#self.gui.subsidence()
			#logging.debug('meas disp : points({}x):{} subs({}x):{}'.format(len(points),points[0:self.LINES],len(subs),subs[0:self.LINES]))				
		except:
			helper.handle_exception()

	def handle_primitives( self, res_list ):
		try:
			ret = True
			cmd_id,res=res_list[0]
				
			if cmd_id == GI.Cmd.Res.Available:
				try:
					ret = res[ 0 ][ 0 ] # Positions: 1: envelop | 2: item itself
					#logging.info('workflow IT2 : GI.Cmd.Res.Available : res: {}'.format( ret ) )
				except:
					helper.handle_exception()
			if cmd_id == GI.Cmd.DB.Material.List:
				try:
					list = res[ 0 ]
					elems = list.split( GI.Value.ListDelimiter )
					logging.debug('cmd: {} res: {}'.format( GI.Cmd.name[ cmd_id ], elems ) )
				except:
					helper.handle_exception()
			else:
				logging.debug('cmd: {} NOT IMPLEMENTED (Primitive)'.format( GI.Cmd.name[ cmd_id ] ) )
				#logging.debug('res_list: {}'.format( res_list ) )

			return ret
		except:
			helper.handle_exception()
