import logging
import Queue
from geomec_interface import GeomecInterface as GI # GI.Cmd

import numpy as np
import helper
import ttk # Progress bar
import Tkinter as tk # tk.CENTER
import tkFont #underline

from time import sleep, time #used in send() and class 'Test'

import threading

NOTHING=99.99

class Controls():

	def __init__( self, gui ):
		self.gui = gui
	
	@staticmethod
	def value( control, val=NOTHING ): # TODO: verify, why I can't I reference to a member and I'm force to do the var global?
		try:
			if isinstance(	 control, tk.Entry ):		res=Controls.value_en( control, val )
			elif isinstance( control, tk.Label ):		res=Controls.value_lb( control, val )
			elif isinstance( control, tk.Radiobutton ):	res=Controls.value_rb( control, val )	# var so this case is never run
			elif isinstance( control, tk.Checkbutton ):	res=Controls.value_ch( control, val )	# var so this case is never run
			elif isinstance( control, ttk.Combobox ):	res=Controls.value_cb( control, val )
			else:
				try: # checkbutton and radiobuttonvar var
					res=Controls.value_ch( control, val )
				except:
					logging.error('Controls : get value : ERROR : {} {}'.format( type( control ), __name__ ))

			if val == NOTHING:
				return res
		except:
			helper.handle_exception()

	@staticmethod # combobox 
	def value_cb( control, val=NOTHING ): #index
		try:
			if val == NOTHING:
				return control.current() if control.get() else GI.Value.NotDefined
			else:
				control.current( val )
		except:
			helper.handle_exception()

	@staticmethod # combobox text 
	def value_cb_text( control ):
		try:
			return control.get()
		except:
			helper.handle_exception()

	@staticmethod # label
	def value_lb( control, val=NOTHING ):
		if val == NOTHING:
			return int( control.get() ) if control.get() else 0
		else:
			control['text']=val

	@staticmethod # entry
	def value_en( control, val=NOTHING ):
		if val == NOTHING:
			return Controls.value_lb( control )
		else:
			control.delete(0,tk.END)	
			control.insert(0,val)	

	@staticmethod # checkbox
	def value_ch( control_var, val=NOTHING ):
		if val == NOTHING:
			return control_var.get()
		else:
			# next is for the control itself, not for the linked var
			#if val:	control_var.select()
			#else:	control_var.deselect()
			control_var.set( val )
			

	@staticmethod # radiobutton
	def value_rb( control_var, val=NOTHING ):

		if val == NOTHING:
			res=control_var.get()
			return int( res )
		else:	
			control_var.set( int( val ) )

	@staticmethod
	def color( control_, color_id ):
		try:
			control_['bg']=color_id
		except:
			helper.handle_exception()

	@staticmethod
	def underline( control_ ): # verified just with labels
		try:
			f = tkFont.Font( control_, control_.cget('font'))
			f.configure(underline = True)
			control_.configure(font=f)
		except:
			helper.handle_exception()

class Params():

	def __init__( self, gui ):
		self.gui=gui
		self.controls = Controls( gui )

	def IT1_AUTO( self, cmd_id, p ):
		try:
			params = ()
			if cmd_id in [	GI.Cmd.IT1.List.Reservoir,
							GI.Cmd.IT1.List.Depletion,
							GI.Cmd.IT1.List.Measured_Displacement,
							GI.Cmd.IT1.Exe.Run_Geomec,
							GI.Cmd.IT1.Exe.Run_Geomec_Step,
							GI.Cmd.IT1.Exe.Wait_End_Run_Geomec ]:
				pass # -> NO params
			elif cmd_id == GI.Cmd.IT1.List.Surface:					params = ( p['res'] )
			elif cmd_id == GI.Cmd.IT1.Get.Reservoir:				params = ( p['res'], p['sur'] )
			elif cmd_id == GI.Cmd.IT1.Get.Displacement:				params = ( p['res'], p['sur'], p['dep'], p['composite_idx'] ) # '-1' : depletion_id # -1: Last depletion stage
			elif cmd_id == GI.Cmd.IT1.Get.Measured_Displacement:	params = ( p['res'], p['composite_idx'], p['sur'] )
			elif cmd_id == GI.Cmd.IT1.Set.Depletion:				params = ( p['res'], p['dep_type'], p['dep'], p['dep_output'] ) #depletion_id = -1 # -1: Last depletion stage ... is_branch = 1
			elif cmd_id == GI.Cmd.IT1.Exe.Remove_Depletion:			params = ( p['dep'] )
			#elif cmd_id == GI.Cmd.IT1.Material_Parameter:		params = ( self.controls.res, GI.Prop.Mat.PoissonsRatio, mat_param_id, self.controls.sur ) # 0: Depletion stage # NOT USED

			return params 
		except:
			helper.handle_exception()

	def IT2_AUTO( self, cmd_id, p ):
		try:
			params = [] # TODO: verify () -> []

			if cmd_id == GI.Cmd.Model.Elems:				params = ( p['res'], p['sur'] )
			elif cmd_id == GI.Cmd.Model.Points:				params = ( p['res'], p['sur'] )
			elif cmd_id == GI.Cmd.Model.Prop.Form:			params = ( p['res'], p['sur'], p['prop_form'] )
			elif cmd_id == GI.Cmd.Model.Prop.Mat:			params = ( p['res'], p['sur'], p['dep'], p['prop_mat'] )
			elif cmd_id == GI.Cmd.Model.Prop.Dep:			params = ( p['res'], p['sur'], p['dep'], p['prop_dep'] )
			elif cmd_id == GI.Cmd.Model.Depletion.Add:		params = ( p['dep_output'] )
			elif cmd_id == GI.Cmd.Model.Depletion.Remove:	params = ( p['dep'] )
			elif cmd_id == GI.Cmd.Model.Depletion.Output:	params = ( p['dep'] , p['dep_output'] ) # (id,output)  (1)id -> -1:last |-2:all ... (2)output  -> = 1: branch | 0: results 
			elif cmd_id == GI.Cmd.Res.Values:				params = ( p['composite'], p['component'], p['composite_idx'], p['dep'] )
			elif cmd_id == GI.Cmd.Res.Available:			pass
			elif cmd_id == GI.Cmd.DB.Points:				params = ( p['composite'], p['composite_idx'] ) 
			elif cmd_id == GI.Cmd.DB.Values:				params = ( p['composite'], p['composite_idx'], p['component'] )
			elif cmd_id == GI.Cmd.DB.Elems:					params = ( p['res'], p['sur'], p['composite'], p['composite_idx'] )
			elif cmd_id == GI.Cmd.DB.PointSet.Create:		params = ( p['ps_id'] )
			elif cmd_id == GI.Cmd.DB.PointSet.AddValues:	params = ( p['ps_id'], p['dep_type'] ) # ( id, depletion_type ) 
			elif cmd_id == GI.Cmd.DB.PointSet.Remove:		params = ( p['ps_id'] )
			elif cmd_id == GI.Cmd.DB.PointSet.Link:			params = ( p['ps_id'], p['composite_idx'], p['res'], p['dep'] )
			elif cmd_id == GI.Cmd.DB.PointSet.Unlink:		params = ( p['ps_id'], p['composite_idx'], p['res'], p['dep']  )
			elif cmd_id == GI.Cmd.DB.PointSet.Values:		params = ( p['ps_id'], p['composite_idx']  )
			elif cmd_id == GI.Cmd.DB.PointSet.ValuesList:	params = ( p['ps_id'] )
			elif cmd_id == GI.Cmd.DB.PointSet.RemoveValues:	params = ( p['ps_id'], p['composite_idx']  )
			elif cmd_id == GI.Cmd.DB.PointSet.ReplaceValues:params = ( p['ps_id'], p['composite_idx']  )
			elif cmd_id == GI.Cmd.Gen.List:					params = ( p['list'] ) #( type_, param ) # param: (1)if BD_Composite then composite else (2)if Surface then reservoir (3)else not needed

			elif cmd_id == GI.Cmd.DB.Material.List:			pass
			elif cmd_id == GI.Cmd.DB.Material.Create:		params = ( p['mat_type'] )
			elif cmd_id == GI.Cmd.DB.Material.Remove:		params = ( p['mat_idx'] )
			elif cmd_id == GI.Cmd.DB.Material.GetValues:	params = ( p['mat_idx'] )
			elif cmd_id == GI.Cmd.DB.Material.SetValues:	params = ( p['mat_idx'] )
			elif cmd_id == GI.Cmd.DB.Material.Link:			params = ( p['mat_idx'], p['res'] )
			elif cmd_id == GI.Cmd.DB.Material.Unlink:		params = ( p['mat_idx'], p['res'] )


			return params
		except:
			helper.handle_exception()

class Progress():

	def __init__( self, gui ):
		self.gui = gui

	def show( self ):
		self.bar = ttk.Progressbar( self.gui, orient="horizontal",	length=200, mode="indeterminate" )
		self.bar.place(relx=0.5, rely=0.5, anchor=tk.CENTER)
		self.bar.start()

	def hide( self ):
		self.bar.stop()
		self.bar.destroy()

class Popup(): # NOT USED

	def __init__( self ):
		pass

	def _center(toplevel):

		toplevel.update_idletasks()
		w = toplevel.winfo_screenwidth()
		h = toplevel.winfo_screenheight()
		size = tuple(int(_) for _ in toplevel.geometry().split('+')[0].split('x'))
		x = w/2 - size[0]/2
		y = h/2 - size[1]/2
		toplevel.geometry("%dx%d+%d+%d" % (size + (x, y)))

	def popup( self, text_ ):
		toplevel = tk.Toplevel() # FIXME: ???? 'self' instead of 'toplevel' ?
		label = tk.Label(toplevel, text=text_, height=0, width=100)
		#label = tk.Label(self.gui, text=text_, height=0, width=100)
		label.pack()
		#label.place(relx=0.5, rely=0.5, anchor=tk.CENTER) # doesn't work
		#_center( toplevel ) # doesn't work

class Data():

	@staticmethod
	def get( cmd_id, gui ):

		try:
			from depletion import Depletion

			res=None

			dep_type = Controls.value_rb( gui.radios['dep_type'] )
			logging.info('iface : Data : get : dep_type: {}'.format( GI.Depletion.List[ dep_type ] ) )

			if		cmd_id == GI.Cmd.IT1.Set.Depletion:			res=Depletion.data(		dep_type, gui )
			elif	cmd_id == GI.Cmd.DB.PointSet.Create:		res=Depletion.points(	dep_type, gui )
			elif	cmd_id == GI.Cmd.DB.PointSet.AddValues:		res=Depletion.values(	dep_type, gui )
			elif	cmd_id == GI.Cmd.DB.PointSet.ReplaceValues:	res=Depletion.values(	dep_type, gui )
			"""
			elif	cmd_id == GI.Cmd.DB.Material.SetValues:		

					from material import params_list, def_values_list;
					values_mod = def_values_list[ GI.Material.Model.Linear ]
					values_mod[ GI.Material.Parameter.Youngs_Modulus ] = 10001
					res=( params_list[ model_id ], values_mod  )		# FIXME: hard-coded for 'Linear' mdoel type
			"""
			#logging.debug( 'Data : get : res:{}'.format( res ) )
			return res

		except:
			helper.handle_exception()

#http://stackoverflow.com/questions/16745507/tkinter-how-to-use-threads-to-preventing-main-event-loop-from-freezing

class ThreadedTask(threading.Thread):

	def __init__(self, queue, req ):

		threading.Thread.__init__(self)
		self.queue = queue
		self.req=req

	def run(self):

		try:
			GI.send_cmd( self.req )
			cmd_id = self.req[ 0 ]
			if cmd_id ==  GI.Cmd.Control.Ini:
				res=GI.get_result()
				self.queue.put( res )
		except:
			helper.handle_exception()

class Commands():

	def __init__( self, gui ):

		try:
			self.gui = gui

			self.params = Params( gui )
			self.progress = Progress( gui )
		except:
			helper.handle_exception()

	def build_seq( self, cmds, params_dict ):

		res=[]
		for cmd in cmds:
			res.append( self.build( cmd,params_dict ) )
		return res

	def build( self, cmd_id, params_dict ):

		try:
			logging.debug('cmd : build : {}'.format( GI.Cmd.name[ cmd_id ] ) )

			params = self.params.IT1_AUTO( cmd_id, params_dict ) if cmd_id < GI.Cmd.Model.Elems else self.params.IT2_AUTO( cmd_id, params_dict )

			if cmd_id  == GI.Cmd.DB.Material.SetValues:

				from material import params_list
				# convert items in list of items if it's not a list
				idxs = params_dict['param_idx'] if isinstance( params_dict['param_idx'], list ) else [ params_dict['param_idx'] ]
				vals = params_dict['param_value'] if isinstance( params_dict['param_value'], list ) else [ params_dict['param_value'] ]
				model_id = params_dict['mat_type']
				# idx -> id
				ids=[]
				values=[]
				for idx, val in zip( idxs, vals ):
					ids.append( params_list[ model_id ][ idx ] )
					values.append( val )
				# arrays with the expected format for CPython: numpy and type specified
				data = ( np.array( ids, np.int ), np.array( values, np.float ) ) # ATTENTION: it must by a numpy array and 'dtype' must be specified
				cmd	= ( cmd_id, params, data )

			elif cmd_id in [	GI.Cmd.IT1.Set.Depletion,
								GI.Cmd.DB.PointSet.Create,
								GI.Cmd.DB.PointSet.AddValues,
								GI.Cmd.DB.PointSet.ReplaceValues]:

				cmd	= ( cmd_id, params, Data.get( cmd_id, self.gui ) )

				if cmd_id == GI.Cmd.IT1.Set.Depletion:
					self.gui.workflow.show('Depletion step set -- Next step: RUN GEOMEC\n','OK')

			else:

				cmd	= ( cmd_id, params )


			logging.debug( cmd )
			return cmd


		except:
			helper.handle_exception()

	def send( self, cmd_list ):

		try:

			logging.debug('Commands : send : cmd_list:{}'.format( cmd_list ) )

			IniCmd = ( GI.Cmd.Control.Ini, () )
			EndCmd = ( GI.Cmd.Control.End, () )

			cmds = [ IniCmd ] + cmd_list + [ EndCmd ]

			logging.debug('-->')
			self.print_( cmd_list )

			self.progress.show()

			self.queue = Queue.Queue() # ???? new queu each time?

			num_cmd = len( cmds )
			for i in range( num_cmd ):
				cmd=cmds[ i ]
				#logging.debug('cmd:{}'.format( cmd ) )
				ThreadedTask( self.queue, cmd ).start()
				sleep(0.2) # Time in seconds. #FIXME

			self.gui.after( GI.Value.PollingInterval, self.on_receive )

		except:
			helper.handle_exception()

	def on_receive(self):
		'''
		This emethod is called every 'GI.Value.PollingInterval' time to check if the response to a request is available.
		Default time: 100ms.
		When respnse received it's treated in 'ResultsHandler' class in 'results_handler.py' module
		'''
		try:
			res = self.queue.get(0) # res: tuple( obj1, obj2, ..., objn )
									# obj1 = cmd = (cmd_id,param)

			try:
				self.is_processing_task = False
				#logging.debug('process_queue : res : {}'.format(res))
				self.print_received( res )
				res1=self.gui.workflow.on_result( res )
			
				#if self.gui.workflow.current_cmd != GI.Cmd.IT1.Get.Reservoir: # before that it needs to (1) assign vals to vars and (2) insert GI.Cmd.Operation.Reservoir to be run
				self.gui.workflow.post( res1 )
				#self.gui.workflow.current_cmd=GI.Cmd.NoCmd
				
				self.progress.hide()

			except:
				helper.handle_exception()

		except Queue.Empty:
			self.gui.after( GI.Value.PollingInterval, self.on_receive )

	#FIXME: It doesn't work fine for LISTs and 'ok'/'nok' responses 
	def print_received(self, res): # ( (cmd_id,data), (cmd_id,data),... )
		try:
			n=0
			size=len(res)
			logging.debug('received:')# size:{}'.format(size))
			for elem in res:

				if n == 0 and size == 1:
					cmd_id, data = elem
					if cmd_id in [	GI.Cmd.IT1.Set.Depletion, 
									GI.Cmd.IT1.Exe.Run_Geomec, 
									GI.Cmd.IT1.Exe.Run_Geomec_Step, 
									GI.Cmd.IT1.Exe.Wait_End_Run_Geomec, 
									GI.Cmd.IT1.Exe.Remove_Depletion,
									
									GI.Cmd.IT1.List.Reservoir,
									GI.Cmd.IT1.List.Surface,
									GI.Cmd.IT1.List.Depletion,
									GI.Cmd.IT1.List.Measured_Displacement,

									GI.Cmd.Gen.List ]:

						logging.debug( '{}'.format( data ) )
						break

				e = elem[ 1 ][ 0 ] if len( elem[1] ) == 1 else elem[ 1 ]
				logging.info(' n:{} : ({}x) {}'.format( n, len( e ), e[0:self.gui.workflow.LINES] ) )
				n+=1
		except:
			helper.handle_exception()

	def print_( self, cmd_list ): # FIXME: code duplicate

		try:
			cmd_s = '[ '
			for cmd in cmd_list:
				cmd_s += (GI.Cmd.name[ cmd[0] ] + ', ')
			cmd_s = cmd_s[:-2]
			cmd_s += ' ]'
			logging.debug('{}'.format( cmd_s ))
			#logging.debug('{}'.format( cmd_list ) )

		except:
			helper.handle_exception()
