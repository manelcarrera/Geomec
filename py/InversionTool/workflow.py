import logging
import helper
from geomec_interface import GeomecInterface as GI # GI.Cmd
from interface import Commands, Controls
from cmds_queue import CmdsQueue, Pos
from task import Task
import settings1
from time import time
from datetime import datetime


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

	def params_check( self, cmds_list ):

		'''
		MCR: 2018-06-17
		TODO: needed to specify when -1 is wrong and when it has a meaning
		Do it better looking at 'Params' class funcs 'IT1_AUTO' and 'IT2_AUTO' ('interface.py')
		By now just implemented to verify combos are filled in so cmds linked to buttons don't contain wrong parameters
		Maybe it's better to implement this functionality no by primitive but by high level command (IT1)

		DISABLED BY NOW
		'''
		try:
			return True

			res=True #FIXME

			if self.current_cmd in [	GI.Cmd.IT1.List.Reservoir,
										GI.Cmd.IT1.List.Surface,
										GI.Cmd.IT1.List.Measured_Displacement,
										GI.Cmd.IT1.Get.Reservoir,
										GI.Cmd.IT1.Get.Measured_Displacement,
										GI.Cmd.IT1.Get.Displacement ]:

				logging.debug( 'params_check : cmds_list : {}'.format( cmds_list ) )
				for cmd in cmds_list:			

					cmd_id,params=cmd
					logging.debug( 'params_check : cmd: {} : cmd_id: {} : params:{}'.format( cmd, cmd_id, params ) )
					res=True #FIXME

					if cmd_id == GI.Cmd.Gen.List:
						if isinstance( params, list ):
							if params[ 0 ] in [ GI.List.Surface,			# res
												GI.List.DB_Composite ]:	# composite
								res = params[ 1 ] != -1
						else:
							res = True # GI.List.Reservoir
					elif cmd_id == GI.Cmd.Model.Elems:
						res = (params[ 0 ] != -1 and params[ 1 ] != -1) # res, sur
					elif cmd_id == GI.Cmd.Model.Points:
						res = params[ 0 ] != -1							# composite, composite_idx
					elif cmd_id == GI.Cmd.DB.Values:
						res = params[ 0 ] != -1							# composite, composite_idx, component
					elif cmd_id == GI.Cmd.Res.Values:
						res = params[ 0 ] != -1							# composite, component, composite_idx, dep

					logging.debug( 'params_check( {} ): {}'.format( GI.Cmd.name[ cmd_id ], res ) )
					if not res:
						break

			logging.debug( 'params_check( {} ): {}'.format( GI.Cmd.name[ self.current_cmd ], res ) )
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

	def message( self, cmd_id, res ):

		text=''
		if res: # OK
			if cmd_id == GI.Cmd.IT1.List.Reservoir:		pass #text='Reservoir list loaded -- Next step: "Fill" surface'
			elif cmd_id == GI.Cmd.IT1.List.Surface:		pass #text='Surface loaded -- Next step: "Get" Reservoir'
			elif cmd_id == GI.Cmd.IT1.Get.Reservoir:	text='Loading reservoir... please wait...'
			elif cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	pass #text='Displacement list loaded -- Next step: "Get" Meas. Disp.'
			elif cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:	pass #text='Displacement loaded.'
			elif cmd_id == GI.Cmd.Operation.Invert: text='Running inversion...'
			elif cmd_id == GI.Cmd.IT1.Get.Displacement:	text='Geomec results loaded and residual calculated. Ready for next inversion.' 
			elif cmd_id == GI.Cmd.IT1.Set.Depletion:	text='Depletion step added to Geomec. Run the Geomec model.'
			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec:		text='Running Geomec...'
			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec_Step:	text='Running Geomec...'
		else: # ERROR
			if cmd_id == GI.Cmd.IT1.List.Reservoir:					pass
			elif cmd_id == GI.Cmd.IT1.List.Surface:					text='Select a reservoir formation first ("Fill")'
			elif cmd_id == GI.Cmd.IT1.Get.Reservoir:				text='Select the top reservoir surface first ("Fill")'
			elif cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	text='Load a reservoir first ("Get")'
			elif cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:	text='Load the list of displacement data first ("Fill")'
			elif cmd_id == GI.Cmd.IT1.Get.Displacement:				text='Modelled displacement is already updated'
			elif cmd_id == GI.Cmd.IT1.Set.Depletion:				pass
			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec:				pass
			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec_Step:			pass

		return text

	def update_global_vars( self, cmd_id, res ):

		if res:
			if cmd_id == GI.Cmd.IT1.List.Reservoir:					self.ResListSelected = True
			elif cmd_id == GI.Cmd.IT1.List.Surface:					self.ResSurfSelected = True
			elif cmd_id == GI.Cmd.IT1.Get.Reservoir:				self.ResSelected = True
			elif cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	self.DispSelected = True         
			elif cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:	pass

			elif cmd_id == GI.Cmd.IT1.Get.Displacement:	
				self.Geomec_Displacement_ReLoaded = False
				self.gui.b_execute['state'] = 'active'      

			elif cmd_id == GI.Cmd.IT1.Set.Depletion:	
				self.gui.b_rungm['state'] = 'active'      
				self.gui.b_setdep['state'] = 'disabled'   

			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec:	
				self.gui.b_execute['state'] = 'disabled'
				self.gui.b_rungm['state'] = 'disabled'
				self.Geomec_Displacement_ReLoaded = True

			elif cmd_id == GI.Cmd.IT1.Exe.Run_Geomec_Step:	
				self.gui.b_execute['state'] = 'disabled'
				self.gui.b_rungm['state'] = 'disabled'

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
						gm_results = True if self.cmds_queue.cmds_done[0] == GI.Cmd.IT1.Get.Displacement else False
						if not gm_results:
							if self.gui.subsign not in ['+','-']:
								self.show('ERROR: sign is either + or -\n','ERROR') 
						if self.gui.resgrid:
							if gm_results:
								pass #self.show('Residual displacement calculated, run inversion\n','OK')
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

	def gui_check( self, cmd_id ):
		res = True
		return res # FIXME
		if cmd_id == GI.Cmd.IT1.Get.Reservoir:
			if	self.gui.combos['res'].current() == -1 or self.gui.combos['sur'].current() == -1: # no items in  the combo
				res = False
		if cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:
			if self.gui.combos['composite_idx'].current() == -1:
				res = False
		#if cmd_id == GI.Cmd.IT1.Get.Displacement:
		#	pass
		return res

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

		if cmd_id	== GI.Cmd.IT1.List.Reservoir:				cb_ = self.gui.combos['res']
		elif cmd_id == GI.Cmd.IT1.List.Surface:					cb_ = self.gui.combos['sur']
		elif cmd_id == GI.Cmd.IT1.List.Depletion:				cb_ = self.gui.combos['dep'] # ?????????
		elif cmd_id == GI.Cmd.IT1.List.Measured_Displacement:	cb_ = self.gui.combos['composite_idx']

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

			if self.current_cmd in [	GI.Cmd.IT1.Set.Depletion, 
										GI.Cmd.IT1.Exe.Run_Geomec, 
										GI.Cmd.IT1.Exe.Run_Geomec_Step, 
										GI.Cmd.IT1.Exe.Wait_End_Run_Geomec,
										GI.Cmd.IT1.Exe.Remove_Depletion ]:
				cmd_id,result = res_list[0] #( (cmd_id,res) )
				if( isinstance(result, basestring) and result == "nok" ):
					logging.debug( 'set/exe cmd res: KO (ERROR)' )
					res=False
				else:
					logging.debug( 'set/exe cmd res: OK (GOOD)' )
			else:
				#Lists
				if self.current_cmd in [	GI.Cmd.IT1.List.Reservoir,
											GI.Cmd.IT1.List.Surface,
											GI.Cmd.IT1.List.Depletion,
											GI.Cmd.IT1.List.Measured_Displacement]: 
						res = res_list[0]
						cmd_id,vals=res
						cb = self.combo( self.current_cmd )
						self.fill_combo(vals,cb)
				#Main commands				
				elif self.current_cmd == GI.Cmd.IT1.Get.Reservoir:				self.set_reservoir(				res_list )
				elif self.current_cmd == GI.Cmd.IT1.Get.Measured_Displacement:	self.set_measured_displacement(	res_list )
				elif self.current_cmd == GI.Cmd.IT1.Get.Displacement:			self.set_displacement(			res_list )

				# Primitives (IT2)
				elif self.is_primitive( self.current_cmd ):							res = self.handle_primitives(	res_list )
			logging.debug('')
		except:
			helper.handle_exception()
			res=False
		finally:
			return res

	# Overidde then in the derived classes
	def set_reservoir(				self, res_list ):	logging.debug( self.TEXT_override )
	def set_displacement(			self, res_list ):	logging.debug( self.TEXT_override )
	def set_measured_displacement(	self, res_list ):	logging.debug( self.TEXT_override )
	def handle_primitives(			self, res_list ):	logging.debug( self.TEXT_override )

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

		elif caller == self.gui.combos[ 'sur' ]:
			self.on_add_cmd( GI.Cmd.IT1.Get.Reservoir, Pos.Begin, {} )

		elif caller == self.gui.combos[ 'composite_idx' ]:
			self.on_add_cmd( GI.Cmd.IT1.Get.Measured_Displacement, Pos.Begin, {} )

		elif caller == self.gui.combos[ 'dep_type' ]:
			if self.gui.inverted:
				self.gui.plotresults()

		elif caller == self.gui.combos[ 'plot' ]:
			if self.gui.resgrid:
				self.gui.setlimits()
				self.gui.plotreservoir()

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




