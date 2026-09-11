import logging
import helper
from geomec_interface import GeomecInterface as GI # GI.Cmd
from interface import Commands, Controls
from cmds_queue import CmdsQueue, Pos
from task import Task
import settings1
from time import time
from datetime import datetime
import numpy as np


class Workflow:

	def __init__( self, gui ):
		try:
			#params
			self.commands = Commands( gui )
			self.gui=gui
			#state
			self.ResSurfSelected = False
			self.ResSelected = False
			self.ResListSelected = False
			self.DispSelected = False
			self.Geomec_Displacement_ReLoaded = True#False

			self.current_cmd = GI.Cmd.NoCmd

			self.TEXT_override = 'To implement (override) in the derived class'
			self.LINES=5

			self.cmds_queue=CmdsQueue( self.gui )
			self.start_time_cmd = time()

			self.force_can = False

		except:
			helper.handle_exception()

	def show(self, text1, text2):
		text1 = datetime.now().strftime('%H:%M:%S') + ": " + text1
		self.gui.textU.insert(1.0,text1,text2)

	def operation_func( self, ope_id ):

		if ope_id == GI.Cmd.Operation.Construct:		func = self.gui.construct
		elif ope_id == GI.Cmd.Operation.Invert:			func = self.gui.inversion
		elif ope_id == GI.Cmd.Operation.Reservoir:		func = self.gui.reservoir
		elif ope_id == GI.Cmd.Operation.Subsidence:		func = self.gui.subsidence
		elif ope_id == GI.Cmd.Operation.LoadOperator:	func = self.gui.load_operator

		return func

	def can( self, cmd_id ):

		try:
			if self.force_can == True:
				res=True
			else:
				res=False
				# Commands
				if		cmd_id == GI.Cmd.IT1.List.Surface:					res = self.ResListSelected
				elif	cmd_id == GI.Cmd.IT1.Get.Reservoir:					res = self.ResListSelected
				elif	cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	res = self.ResSelected
				elif	cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:		res = self.DispSelected
				elif	cmd_id == GI.Cmd.IT1.Get.Displacement:				res = self.Geomec_Displacement_ReLoaded

				elif	cmd_id in [ GI.Cmd.IT1.List.Reservoir,
									GI.Cmd.IT1.Set.Depletion,		
									GI.Cmd.IT1.Exe.Run_Geomec,
									GI.Cmd.IT1.Exe.Run_Geomec_Step,
									GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,

									GI.Cmd.Res.Available ]:					res = True

				# Operations
				# TODO: Implement
				elif cmd_id in [	GI.Cmd.Operation.Construct,
									GI.Cmd.Operation.Invert,
									GI.Cmd.Operation.Reservoir,
									GI.Cmd.Operation.Subsidence ]:	
																			res = True
			logging.debug( 'can( {} ) : {} : forced:{}'.format( GI.Cmd.name[ cmd_id ], res, self.force_can ) )
			return res
		except:
			helper.handle_exception()

	# FIXME: Take care ok keeping this func up to date
	def is_command( self, cmd_id ):
		if cmd_id in GI.Cmd.Operation.List:
			return False
		else:
			return True

	def type( self, cmd_id ):
		res=GI.Cmd.Type.Other
		if cmd_id <= GI.Cmd.Gen.List:
			res=GI.Cmd.Type.Command
		elif cmd_id in GI.Cmd.Operation.List:
			res=GI.Cmd.Type.Operation
		return res

	def is_primitive( self, cmd_id ):
		if cmd_id >= GI.Cmd.Model.Elems and cmd_id <= GI.Cmd.Gen.List:
			return True
		else:
			return False

	def pre( self, cmd_id ): 
		'''actions to be performed before cmd execution'''

		try:
			self.start_time_cmd = time()

			logging.debug( 'pre( {} )'.format( GI.Cmd.name[ cmd_id ] ) )

			#logging.debug(' cmds : queue : {}'.format( CmdsQueue.cmds_s( self.cmds ) ))

			if self.gui.control:
				self.gui.control.update_cmd_doing( cmd_id )
				self.gui.control.update_cmds_queue()

			if cmd_id in [	GI.Cmd.Operation.Invert,
							GI.Cmd.Operation.Subsidence ] :
				self.gui.widget.getinput()

		except:
			helper.handle_exception()

	def post( self, res ): 
		'''actions to be performed after cmd execution'''

		try:
			cmd_id=self.current_cmd # FIXME: don't pass it as a parameter, make funtions use the global var instead

			if self.gui.control:
				self.gui.control.add_cmd_done( cmd_id, CmdsQueue.elapsed_time( self.start_time_cmd ), self.cmds_queue.params_doing )

			logging.debug( 'post( {} ) : {}'.format( GI.Cmd.name[ cmd_id ], res ) )

			if cmd_id == GI.Cmd.Operation.Pause:
				pass
			else:
				# (a) Commands
				if self.type( cmd_id ) == GI.Cmd.Type.Command:
				
					text=self.message( cmd_id, res )
					if text:
						self.show(text+'\n','INFO' if res else 'ERROR')

					self.update_global_vars( cmd_id, res )
					# res 'ok'
					if res:

						if self.current_cmd == GI.Cmd.IT1.Get.Reservoir:
							self.cmds_queue.add(GI.Cmd.Operation.Reservoir, Pos.Begin)

						elif self.current_cmd == GI.Cmd.IT1.Get.Displacement or self.current_cmd == GI.Cmd.IT1.Get.Measured_Displacement:
							self.cmds_queue.add(GI.Cmd.Operation.Subsidence, Pos.Begin)
					# res 'nok'
					else: 
						# TODO: do control retries
						if self.current_cmd == GI.Cmd.Res.Available:
							self.gui.after( 5*1000, lambda: self.on_add_cmd( GI.Cmd.Res.Available ) )
				# (b) Operations
				elif self.type( cmd_id ) == GI.Cmd.Type.Operation:
					if self.current_cmd == GI.Cmd.Operation.Construct:
						gm_results = True if self.cmds_queue.cmds_done[0] == GI.Cmd.IT1.Get.Displacement else False
						if res:
							self.gui.b_execute['state'] = 'normal'
							self.show('Operator constructed. Run the inversion.\n','INFO')
						else:
							if self.gui.subsgrid:
								if self.gui.resgrid:
									pass
								else:
									self.show('Missing reservoir grid!!\n','highlight')
							else:
								self.show('Missing observation grid!!\n','highlight')
						self.gui.textU.tag_configure('highlight', background='red', font='helvetica 6 bold', relief='raised')
				
					# Invert
					elif self.current_cmd == GI.Cmd.Operation.Invert:

						if res:
							self.gui.plotresults()
							self.gui.b_setdep['state'] = 'active'  
							self.show('Displacement inverted. Next, set Geomec depletion.\n','INFO')
						else:
							self.show('Constructing forward operator (takes several minutes)...\n','highlight')

					# Reservoir
					elif self.current_cmd == GI.Cmd.Operation.Reservoir:
						if res:
							#if self.gui.subsgrid:
								#self.gui.b_construct['state'] = 'normal'
							self.show('Reservoir loaded. Select displacement data.\n','INFO')
							#self.show('Select the displacement data.\n','INFO')
							self.gui.plotreservoir()
						else:
							pass # FIXME
				
					# Subsidence
					elif self.current_cmd == GI.Cmd.Operation.Subsidence:
						#pass
						# threaded ope
						#np.savetxt("subs.txt", self.gui.subs)                        
						gm_results = True if self.cmds_queue.cmds_done[0] == GI.Cmd.IT1.Get.Displacement else False
						if np.all(np.isnan(self.gui.subs)):
							self.show('ERROR: No valid subsidence results from GEOMEC\n','ERROR') 
						else:		
							if not gm_results:
								if self.gui.subsign not in ['+','-']:
									self.show('ERROR: sign is either + or -\n','ERROR') 
							if self.gui.resgrid:
								if gm_results:
									if self.gui.operator_constructed:
										pass
									else:
										self.show('Displacement data changed. Reconstructing forward operator...\n','INFO')
										pass
								else:
									self.show('Displacement data loaded. Constructing forward operator (takes several minutes)...\n','INFO')
								#self.gui.b_construct['state'] = 'normal'
							else:
								self.show('Please import reservoir grid\n','Warn')
							self.gui.textU.tag_configure('Warn', background='yellow', font='helvetica 6 bold', relief='raised')


						if not gm_results: 
							pass
						else:
							self.gui.sub9.clear()
							self.gui.sub9.plot(range(len(self.gui.avs)),self.gui.avs)

						if self.gui.usex or self.gui.usey or self.gui.usez:
				
							#self.gui.setlimits()
							self.gui.plotobservation()
							#self.gui.b_construct['state'] = 'normal'
				
						else:
							self.show('ERROR: indicate at least 1 displacement\n','ERR')
							self.gui.textU.tag_configure('ERR', background='red', font='helvetica 6 bold', relief='raised')

						if res:
							pass
						else:
							pass # FIXME

				#next cmd to run
				if res:
					next = settings1.settings1['next']
					if self.current_cmd in next:
						self.cmds_queue.add( next[ self.current_cmd ], Pos.Begin, {} )


			# Common
			#self.current_cmd=GI.Cmd.NoCmd
			self.cmds_queue.cmds_done.insert(	0, self.cmds_queue.cmd ) #use this everywhere instead of 'self.current_cmd' -> don't mantain two vars
			self.cmds_queue.params_done.insert( 0, self.cmds_queue.params_doing )

			if self.gui.control:
				if Controls.value_ch( self.gui.control.checks['run_one'] ) or cmd_id == GI.Cmd.Operation.Pause:
					Controls.value_ch( self.gui.control.checks['pause'], True )
					self.cmds_queue.paused = True # FIXME: don't mantain two vars, one in workspace and the checkbox control in control.py

			self.cmds_queue.params_doing={}
			self.current_cmd=GI.Cmd.NoCmd # cmd ended so next command can be run
			if self.gui.control:
				self.gui.control.update_cmd_doing( self.current_cmd )

		except:
			helper.handle_exception()

	def on_task( self, cmd_id, task ):
		try:
			logging.info( '++++++++++++++++ on_task( {} ) ++++++++++++++++'.format( GI.Cmd.name[ cmd_id ] ) ) # task.__name__, task.func_name : both good
			#self.current_cmd = cmd_id
			t=Task( self.gui )
			t.exe( task )
		except:
			helper.handle_exception()

	def on_add_cmd( self, cmd_id, pos=Pos.End, params={} ):
		try:
			logging.debug( 'workflow : on_add_cmd : cmd_id: {} : params: {}'.format( GI.Cmd.name[ cmd_id ], params ) )
			if self.gui_check( cmd_id ):
				self.cmds_queue.add( cmd_id, pos, params )
			else:
				pass # show error popup
		except:
			helper.handle_exception()

	def on_btn( self, id, val ):
		try:
			val_s = GI.Depletion.List[ id ] if id == GI.GUI.DepType else GI.Plot.List[ id ]
			logging.debug( 'workflow : on_btn : {} -> {}'.format( GI.GUI.List[ id ], val_s ) )
			if id == GI.GUI.DepType:
				if self.gui.inverted:
					self.gui.plotresults()
			elif id == GI.GUI.Plot:
				if self.gui.resgrid:
					self.gui.setlimits()
					self.gui.plotreservoir()
		except:
			helper.handle_exception()

	def on_exit_cmds_queue( self ):

		try:
			exit = Controls.value_ch( self.gui.control.ch_exit_test_val )
			logging.debug( 'On_Exit_Test : cmds : current:{} context:{} : exit:{}'.format( self.cmds_queue.cmds, self.cmds_queue.context.cmds, exit ) )
			# FIXME: crash
			#exit2 = Controls.value_ch( self.gui.ch_exit_test_val )
			#logging.debug( 'On_Exit_Test : cmds : current:{} context:{} : exit:{} exit2:{}'.format( self.test_runner.cmds, self.test_runner.context.cmds, exit, exit2 ) )
			if exit == 1:
				#backup
				if len( self.cmds_queue.cmds ) > 0:
					logging.debug( 'On_Exit_Test : {} : backup'.format( exit ) )
					self.cmds_queue.context.save()

					self.cmds_queue.cmds=[]
					self.current_cmd = GI.Cmd.NoCmd
			else:
				#restore backup
				if len( self.cmds_queue.context.cmds ) > 0:
					logging.debug( 'On_Exit_Cmds_Queue : {} : restore'.format( exit ) )
					self.cmds_queue.context.restore()
		except:
			helper.handle_exception()

	def combo( self, cmd_id ):

		cb_ = None

		if cmd_id	== GI.Cmd.IT1.List.Reservoir: cb_ = self.gui.combos['combo_id']

		return cb_

	def fill_combo( self, res, combo ):

		try:
			if len( res ):
				list = res[ 0 ]
				elems = list.split( GI.Value.ListDelimiter ) #list: tuple -> list[0]: first and only elem of the tuple
				#logging.debug( '{}'.format( elems )  )
				combo['values'] = elems
				combo.current( 0 )
		except:
			helper.handle_exception()

	def on_result( self, res_list ): # res_list : ( (cmd_id,res),(cmd_id,res),... )
		
		try:
			res=True
			logging.debug('<--')
			self.commands.print_( res_list )

			# possible responses: 'ok' / 'nok'
			if self.current_cmd in [ ]:
				cmd_id,result = res_list[0] #( (cmd_id,res) )
				if( isinstance(result, basestring) and result == "nok" ):
					logging.debug( 'set/exe cmd res: KO (ERROR)' )
					res=False
				else:
					logging.debug( 'set/exe cmd res: OK (GOOD)' )
			else:
				#Lists
				if self.current_cmd in []: 
						res = res_list[0]
						cmd_id,vals=res
						cb = self.combo( self.current_cmd )
						self.fill_combo(vals,cb)
				#Main commands				
				elif self.current_cmd == GI.Cmd.IT1.Get.Reservoir:				self.set_reservoir(				res_list )

				# Primitives (IT2)
				elif self.is_primitive( self.current_cmd ):							res = self.handle_primitives(	res_list )
			logging.debug('')
		except:
			helper.handle_exception()
			res=False
		finally:
			return res

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

	def on_combo_new_selection(self, event):
		'''
		Triggers an event on any change in any of the test purpose combo boxes:
			Reservoir
			Depletion
			Displacement (measured)
		'''
		
		caller = event.widget

		if caller == self.gui.combos[ 'res' ]:
			self.on_add_cmd( GI.Cmd.IT1.List.Surface, Pos.Begin, {} )

	def on_key_release(self,e):
		'''
		Method implemented just for testing.
		A comamnd is sent depending on the function key pushed.
		Implemented in 'KeyEventHandler' class in 'key_event_handler.py' module 
		'''
		try:
			from key_event_handler import KeyEventHandler as handler
			handler.handle( self.gui, e )
		except:
			helper.handle_exception()




