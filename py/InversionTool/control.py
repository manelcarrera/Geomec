import logging
import helper
from Tkinter import Frame
import Tkinter as tk
import ttk
from geomec_interface import GeomecInterface as GI # GI.Cmd
from interface import Controls, NOTHING
from cmds_queue import CmdsQueue, Pos #FIXME: move 'time_format' func to a generic class
from time import time, sleep
import tkFont
import os


# flash -> blink
# https://stackoverflow.com/questions/32487516/how-to-flash-tkinter-label-limited-number-of-times-and-only-if-conditions-are-m

class Control:
	class Action:
		Remove_ALL		= 0
		Remove_LAST		= 1
		Other			= 2
		Clear_DONE		= 3
		Test			= 4
		Pause			= 5
		Settings		= 6
		Radio			= 7
		Add				= 8	# add cmd
		Send			= 9
		Params			= 10
		Remove_FIRST	= 11
		Reset			= 12
		Remove			= 13
		Insert			= 14
		Save			= 15
		ClearLog		= 16
		Errors			= 17
		Operator_Save	= 18
		Operator_Load	= 19

		List = ['Remove_ALL', 'Remove_LAST', 'Other', 'Clear_DONE', 'Test', 'Pause', 'Settings', 'Radio', 'Add', 'Send', 'Params', 'Remove_FIRST', 'Reset', 'Remove', 'Insert', 'Save', 'ClearLog', 'Errors', 'Operator_Save', 'Operator_Load']

class Sender:

	class Values:
		Default = 0
		Current = 1

	def __init__( self, gui ):

		try:
			self.gui = gui
			self.CR = '\n'
			self.MAX_CMD=20

			self.frames=[]

			self.radios={} # self.radios[ name ] = { 'frame' : frame, 'var' : var }
			self.radios_def = [
				# id						list						default_value				frame idx (and col idx)
				[ GI.Cmd.Name1,				GI.Cmd.List1,				GI.Cmd.IT1.List.Reservoir	, 0		],
				[ GI.Cmd.Name2,				GI.Cmd.List2,				-1							, 1		],
				[ GI.Cmd.Name3,				GI.Cmd.List3,				-1							, 2		],
				[ GI.Cmd.Name4,				GI.Cmd.List4,				-1							, 2		],

				[ GI.Composite.Name,		GI.Composite.List,			GI.Composite.Displacement	, 3		],
				[ GI.Component.Name,		GI.Component.List,			GI.Component.Z				, 4		],
				[ GI.Prop.Form.Name,		GI.Prop.Form.List,			GI.Prop.Form.Thickness		, 4		],
				[ GI.Prop.Mat.Name,			GI.Prop.Mat.List,			GI.Prop.Mat.PoissonsRatio	, 4		],
				[ GI.Prop.Dep.Name,			GI.Prop.Dep.List,			GI.Prop.Dep.PorePressure	, 4		],
				[ GI.List.Name,				GI.List.List,				GI.List.Reservoir			, 4		],
				[ GI.Depletion.Name,		GI.Depletion.List,			GI.Depletion.Pore_Pressure	, 5		],
				[ GI.Depletion.Id.Name,		GI.Depletion.Id.List,		-GI.Depletion.Id.Last-1		, 5		],		# ATENTION !!!
				[ GI.Depletion.Output.Name, GI.Depletion.Output.List,	GI.Depletion.Output.Branch-1, 5		],		# ATENTION !!!
				[ GI.Depletion.Action.Name, GI.Depletion.Action.List,	GI.Depletion.Action.New		, 5		],
				[ GI.Material.Model.Name,	GI.Material.Model.List,		GI.Material.Model.Linear	, 6		]
			]


			self.entries_keys = ['cmd','list','res','sur', 'composite_idx','ps_id','dep','composite', 'component','dep_type','dep_output','dep_action','prop_form','prop_mat','prop_dep','mat_idx', 'mat_type','param_idx', 'param_value']

			self.entries_keys_non_radio = ['list','res','sur','composite_idx','ps_id', 'mat_idx', 'param_value']

			self.equivalences_radios_entries={	
				GI.Cmd.Name1			:	'cmd',		
				GI.Cmd.Name2			:	'cmd',		
				GI.Cmd.Name3			:	'cmd',		
				GI.Cmd.Name4			:	'cmd',		
				GI.Composite.Name		:	'composite',
				GI.Component.Name		:	'component',
				GI.Prop.Form.Name		:	'prop_form',
				GI.Prop.Mat.Name		:	'prop_mat',
				GI.Prop.Dep.Name		:	'prop_dep',
				GI.List.Name			:	'list',
				GI.Depletion.Name		:	'dep_type',
				GI.Depletion.Id.Name	:	'dep',
				GI.Depletion.Output.Name :	'dep_output',
				GI.Depletion.Action.Name :	'dep_action',
				GI.Material.Model.Name :	'mat_type',
				GI.Material.Parameter.Name : 'param_idx'
			}

			self.default_values = {
				'cmd'				: GI.Cmd.IT1.List.Reservoir,		# radio
				'list'				: GI.List.Reservoir,				# radio
				'res'				: GI.Value.Default.ReservoirId,				# text
				'sur'				: GI.Value.Default.SurfaceId,				# text
				'dep'				: GI.Depletion.Id.Last,				# radio
				'composite_idx'		: 0,										# text
				'ps_id'				: 0,										# text
				'composite'			: GI.Composite.Displacement,		# radio
				'component'			: GI.Component.Z,					# radio
				'depletion_type'	: GI.Depletion.Pore_Pressure,		# radio
				'output'			: GI.Depletion.Output.Branch,		# radio
				'prop_form'			: GI.Prop.Form.Thickness,			# radio # text
				'prop_mat'			: GI.Prop.Mat.PoissonsRatio,		# radio
				'prop_dep'			: GI.Prop.Dep.PorePressure,			# radio
				'mat_type'			: GI.Material.Model.Linear,			# radio
				'mat_idx'			: -1,										# text
			}

			self.entries={}	# entry_id: tk.Entry						# bottom frame
			self.inputs=[]	# items: { 'label': val1, 'entry': val2 }	# top_frame

			self.checks={}	# id -> linked var

			self.labels={}	# status

			self.btns={}

			self.help={
				Control.Action.Remove_ALL	: '',
				Control.Action.Remove_LAST	: '',
				Control.Action.Other		: '',
				Control.Action.Clear_DONE	: '',
				Control.Action.Test			: '',
				Control.Action.Pause		: '',
				Control.Action.Settings		: '',
				Control.Action.Radio		: '',
				Control.Action.Add			: '',
				Control.Action.Send			: '',
				Control.Action.Params		: '',
				Control.Action.Remove_FIRST	: '',
				Control.Action.Reset		: '',
				Control.Action.Remove		: '',
				Control.Action.Insert		: '',
				Control.Action.Save			: 'Printsthemmands sequece tothe logs file, including parameters, to be reused easy; copy it in test_definitions.py and do remove the colonsfrom the commandsids',
				Control.Action.ClearLog		: 'Clears log file',
				Control.Action.Errors		: 'Looks for exceptions in the log file',
				Control.Action.Operator_Save: 'Saves the operator to a file',
				Control.Action.Operator_Load: 'Loads the operator from a file' }

			self.vars={}
			self.vars['dir']=os.path.dirname( ( __file__ ) )

			self.create()
			self.show( True ) #FIXME

			self.on_btn( Control.Action.Pause )		# just to update the var
			self.on_btn( Control.Action.Settings )	# FIXME: move to GUI?
			self.on_btn( Control.Action.Reset )		# to fill entries adnd set colors

			# FIXME: THIS CRASHES
			#self.on_btn( Control.Action.Radio, GI.Material.Model.Name )
			#self.update_mat_model_params()
			#self.root.after( 5*1000, lambda: self.on_btn( Control.Action.Radio, GI.Material.Model.Name ) ) # FIXME

			#self.radios[ GI.Material.Model.Name ]['var']=0

		except:
			helper.handle_exception()

		#self.root.after( 5*1000, lambda: self.gui.workflow.on_add_cmd( GI.Cmd.Operation.Test ) ) # FIXME

	def update_mat_model_params( self ):

		try:
			from material import models_def, MaterialExt

			radio = self.radios[ GI.Material.Model.Name ]['var']
			model_id = Controls.value_rb( radio )

			params_desc=[ row[ MaterialExt.Model.Param.Pos.Desc ] for row in models_def[ model_id ][ MaterialExt.Model.Definition.Params ] ]

			col=7

			if col < len( self.frames ):
				for radio in self.frames[ col ].pack_slaves():
					radio.destroy()
			#self.create_radiobutton( self.bottom_, 'Model params', params_desc, 0, col )	# ( parent, name, values, default_value, parent_frame_idx ):
			self.create_radiobutton( self.right, GI.Material.Parameter.Name, params_desc, 0, col )	# ( parent, name, values, default_value, parent_frame_idx ):

			#self.frames[ col ].grid( row=0, column=col+1, sticky=tk.N )
			self.frames[ col ].grid( row=0, column=0, sticky=tk.N )
		except:
			helper.handle_exception()


	def update_params_labels( self, cmd_id ): 
		try:
			for input in self.inputs:
				input['label'][ 'text' ]=''

			num_params=len(GI.Cmd.params[cmd_id])
			for i in range( num_params ):
				if num_params > i:	
					self.inputs[ i ]['label'][ 'text' ] = GI.Cmd.params[ cmd_id ][ i ]
		except:
			helper.handle_exception()

	def value_def( self, value_id ):
		try:
			value=''
			if value_id in self.default_values:
				value=self.default_values[ value_id ]
			return value
		except:
			helper.handle_exception()

	def values_def( self ):
		try:
			#return self.default_values.values()
			return self.default_values
		except:
			helper.handle_exception()

	# FIXME: different keys names
	def value( self, id ): # both entries and radiobuttons

		res=99
		if id in self.entries:
			res = Controls.value_en( self.entries[ id ] )
		elif id in self.radios:
			res = Controls.value_rb( self.radios[ id ]['var'] )
		return res

	def values( self ):
		'''returs a dict with entries values; some entries values must have been filled in wijth radiobuttons selections'''

		try:
			res={}
			for id, control in self.entries.iteritems():	# { 'name': entry_wg }		
				res[ id ] =  Controls.value_en( control )
			return res
		except:
			helper.handle_exception()

	def update_cmd_doing( self, cmd_id ):
		'''shows the command currently being executed'''
		try:
			logging.debug('sender : add : cmd_doing : {}'.format( GI.Cmd.name[ cmd_id ] ) )
			#self.doing['text'] = GI.Cmd.name[ cmd_id ] + self.CR
			Controls.value_lb( self.labels['doing'], GI.Cmd.name[ cmd_id ] + self.CR)
		except:
			helper.handle_exception()

	def update_elepsed_time( self ):
		'''updates current command elapsed time so far'''
		try:
			#self.total_time['text'] = elapsed_time
			Controls.value_lb( self.total_time, CmdsQueue.elapsed_time( self.gui.workflow.start_time_cmd ) )
		except:
			helper.handle_exception()

	def add_cmd_done( self, cmd_id, elapsed_time, params ):
		'''cmd just finished is added (prepend) to done commands text control'''
		try:
			logging.debug('sender : add : cmd_done : {} : params: {}'.format( GI.Cmd.name[ cmd_id ], params ) )
			self.cmds_done.insert( 1.0, GI.Cmd.name[ cmd_id ] + self.CR )
			self.elapsed_time.insert( 1.0, elapsed_time + self.CR )
			self.params_queue_done.insert( 1.0, '{}{}'.format(params, self.CR) )

			#self.total_time['text'] = CmdsQueue.format_time( time(self.total_time.cget('text')) + time( elapsed_time ) ) # FIXME: addition of all the times in MULTI-TEXT
		except:
			helper.handle_exception()

	def update_cmds_queue( self ):
		'''updates commands queue control with values in commands queue class; commands in queue waiting to be executed'''
		try:
			logging.debug('control : update_cmds_queue : gui.workflow.cmds_queue.cmds: {}'.format( self.gui.workflow.cmds_queue.cmds) )

			self.cmds_queue.delete('1.0', tk.END)
			for cmd_id in self.gui.workflow.cmds_queue.cmds:
				self.cmds_queue.insert( tk.END, GI.Cmd.name[ cmd_id ] + self.CR )

			self.params_queue.delete('1.0', tk.END)
			for param in self.gui.workflow.cmds_queue.params:
				params_s='{}{}'.format( param, self.CR )
				self.params_queue.insert( tk.END, params_s )

		except:
			helper.handle_exception()

	def set_params_default_values( self, cmd_id ): # NOT USED : INPUTS
		try:

			logging.debug('control : set_params_default_values( {} )'.format( GI.Cmd.name[ cmd_id ] ) )

			num_params = len(GI.Cmd.params[cmd_id])

			#edits = [ self.p1, self.p2, self.p3, self.p4, self.p5, self.p6 ]
			for input in self.inputs: #clean
				input['entry'].delete(0,tk.END)	

			for i in range(num_params): #fill
				key = GI.Cmd.params[ cmd_id ][ i ]
				value = self.default_values[ key ] if key in self.default_values else ''
				#logging.debug('key:{} value:{}'.format( key, value ) )
				self.inputs[ i ]['entry'].insert(0,value)	
		except:
			helper.handle_exception()

	def show_toggle( self ):
		'''toggles window visivility : 'iconify' and 'deiconify'''
		#self.shown = not self.shown
		shown = not self.shown
		self.show( shown )

	def show( self, do_show ):
		'''show/hide control window'''
		try:
			self.shown = do_show
			#logging.debug('sender: show:{}'.format( do_show ) )
			if do_show: # show
				self.root.deiconify()
				#self.root.lift()
				#self.root.focus_force() # this gives the focus to both windows
			else: # hide
				self.root.iconify()
				#self.gui.lift() # this alone doesn't do the work
				#self.gui.focus_force()
				# self.root.withdraw() 
		except:
			helper.handle_exception()

	def create( self ):
		'''layout in two frames: top and bottom'''
		try:
			self.root = tk.Toplevel() # FIXME: ???? 'self' instead of 'toplevel' ?
			self.root.geometry('1500x950')

			default_bg = self.root.cget('bg')

			# color names : https://wiki.tcl.tk/37701
			status_	= Frame( self.root, bg='lavender blush',	width=950, height=20) #b g='cyan', 'gray2' padx=3, pady=3
			top_	= Frame( self.root, bg=default_bg,			width=950, height=300) #b g='cyan', 'gray2' padx=3, pady=3
			self.bottom_	= Frame( self.root, bg='lavender blush',	width=950, height=400)
			self.right	= Frame( self.root, bg='lavender blush',	width=400, height=950)

			#ksbar=tk.Scrollbar(self.root, orient=tk.VERTICAL)

			r,c=(0,0)
			status_.grid(	row=0,		column=0, sticky=tk.W)
			top_.grid(		row=1,	column=0, sticky=tk.W)
			self.bottom_.grid(	row=2,	column=0, sticky=tk.W)
			#ksbar.grid(		row=r+2,	column=c+1, rowspan=2, sticky="ns")
			self.right.grid(		row=0,	column=1, rowspan=3, sticky="ns")

			self.status(	status_ )
			self.top(		top_ )
			self.bottom(	self.bottom_ )

			#self.shown=True
			#self.root.bind("<KeyRelease>", self.gui.workflow.on_key_release)
			self.root.protocol("WM_DELETE_WINDOW", self.root.iconify)			
			#self.root.bind('<Escape>', lambda e: root.destroy())
			self.root.bind('<Escape>', self.show_toggle())
			self.root.bind("<F8>", self.gui.workflow.on_key_release)
			#self.root.after( 5*1000, lambda: self.show1( False ) )

		except:
			helper.handle_exception()

	def create_radiobutton( self, parent, name, values, default_value, parent_frame_idx ):
		try:

			if parent_frame_idx >= len( self.frames ):
				self.frames.append( Frame( parent ) ) # no jumps allowed: 1,2,3: ok ...  1,2,4: not ok
			
			frame = self.frames[ parent_frame_idx ]

			label = tk.Label( frame, text=name )

			f = tkFont.Font( label, label.cget('font'))
			f.configure(underline = True)
			label.configure(font=f)

			label.pack(anchor=tk.W) # side=tk.TOP : does't work

			# FIXME: this is ugly, specific things in a generic func
			if name in [ GI.Cmd.Name2, GI.Cmd.Name3, GI.Cmd.Name4 ]:
				var = self.radios[ GI.Cmd.Name1 ]['var']
				# value to assign to the radio button
				if name == GI.Cmd.Name2:	idx = GI.Cmd.Model.Elems			
				elif name == GI.Cmd.Name3:	idx = GI.Cmd.DB.Material.List
				elif name == GI.Cmd.Name4:	idx = GI.Cmd.Operation.Construct
			else:
				#var = tk.StringVar()
				var = tk.IntVar()
				idx=0

			r,c=(0,0)
			for item in values:

				b = tk.Radiobutton( frame, text=item, variable=var, value=idx, command= lambda: self.on_btn( Control.Action.Radio,name ) )
				#b.pack( sticky=tk.N)
				#b.pack(anchor=tk.W)

				""""
				# Look why this hungs : maybe first all widgets creation and then make the layout (grid)
				#div, rest = divmod( idx, 15 )
				if name == GI.Cmd.Name:
				b.grid( row=r, column=c )
				# layout in two columns
				r = r+1 if c == 1 else r
				c = 0 if c == 1 else 1
				else:
				"""
				b.pack(anchor=tk.W)

				idx+=1
			if name in [ GI.Cmd.Name2, GI.Cmd.Name3, GI.Cmd.Name4 ]:
				pass
			else:
				var.set( default_value )	

			self.radios[ name ]	= { 'frame' : frame, 'var' : var } # IMPORTANT

		except:
			helper.handle_exception()

	def create_entry( self, parent, name, r ):
		try:
			label = tk.Label( parent, text=name )
			
			entry = tk.Entry( parent )
			entry.insert( 0, '0')

			c=0
			label.grid(  row=r, column=c )
			entry.grid(	 row=r, column=c+1 )

			self.entries[ name ] = entry # IMPORTANT

		except:
			helper.handle_exception()

	def status( self, parent ):
		try:
			frame = Frame( parent )
			label = tk.Label( frame, bg='lavender blush', text="Staus-top..." )
			label.pack( side=tk.LEFT, fill=tk.X )
			self.labels['status-top']=label

			#frame.grid( row=0,	column=0 )
			frame.pack( side=tk.LEFT, fill=tk.X )
		except:
			helper.handle_exception()

	def top( self, top ):
		"""
			0				1				2				3						4		5				6

			0	'Params'	'Command'						'Queue'					'->'	'Done'			Total time
			1												
			2				Cmds-combo		Btn-send		Current-cmd
			3	Param1		Edit1			Chechbox-Test	Cmds-queue (12rows)				Cmds-done		Elapsed time
			4	Param2						Btn-test
			5	Param3						Ch-exit-test
			6	Param4
			7	Param5
			8	P6			E6
			9	-			Status	

		"""
		try:

			########### (a) Widgets creation ###########

			# Frames
			frame_c1 = Frame( top )	# inputs (not any longer) ... now: btns column
			frame_c2 = Frame( top )	# btnd column
			frame_c3 = Frame( top )	# btns under cmds queue
			frame_c5 = Frame( top )	# btns under done cmds
			frame_c22=Frame( top ) # remove command 'idx''
			frame_c23=Frame( top ) # add command to position
			#frame_c6 = Frame( top )


			# (1) title
			label_R			= tk.Label( top, text="R" )
			label_A			= tk.Label( top, text="A" )
			label_queue		= tk.Label( top, text="Queue" )
			label_done		= tk.Label( top, text="Done" )
			label_arrow		= tk.Label( top, text="->" )
			#self.doing		= tk.Label( top, text="doing...", fg="blue" )
			self.labels['doing'] = tk.Label( top, text="doing...", fg="blue" )
			self.total_time = tk.Label( top, text="00:00" )
			label_sep		= tk.Label( top, text="-" )
			label_pars_2	= tk.Label( top, text="Params" )
			label_sep_2		= tk.Label( top, text="-" )
			label_pars_done	= tk.Label( top, text="Params (Done)" )

			"""
			# cmds combo
			var = tk.StringVar()
			self.cb = ttk.Combobox( top, textvariable=var )
			self.cb.bind("<<ComboboxSelected>>", self.on_cmds_new_selection )
			self.cb['values']=GI.Cmd.name
			self.cb.current( 0 )

			# params
			for i in range(6):
				self.inputs.append( {'label' : tk.Label( frame_c1 ), 'entry' : tk.Entry( frame_c1 ) } )
			"""

			# ULTI-LINES: commands: queue | done | elapsed time
			self.cmds_queue=tk.Text(	top,relief=tk.FLAT,font=("Helvetica", 8), width=30, height=self.MAX_CMD) # height = (it seems) num lines # tk.SUNKEN
			self.cmds_done=tk.Text(		top,relief=tk.FLAT,font=("Helvetica", 8), width=30, height=self.MAX_CMD) 
			self.elapsed_time=tk.Text(	top,relief=tk.FLAT,font=("Helvetica", 8), width=5,  height=self.MAX_CMD)
			self.params_queue=tk.Text(	top,relief=tk.FLAT,font=("Helvetica", 8), width=60, height=self.MAX_CMD)
			self.params_queue_done=tk.Text(	top,relief=tk.FLAT,font=("Helvetica", 8), width=60, height=self.MAX_CMD)	# set depletion cmd is the longest with 6 params

			# ---------------------------> in frames btns

			# -------------> C2 : left side btns

			#btn_send=tk.Button(frame_c2,text='Send',command= lambda: self.on_btn( Control.Action.Send ))

			# check : pause treating commnads in queue
			var=tk.IntVar()
			ch_pause=tk.Checkbutton( frame_c2,text='Pause',onvalue=True,offvalue=False,variable=var,command=lambda: self.on_btn( Control.Action.Pause ))
			#self.ch_pause_var=var
			self.checks['pause']=var
			ch_pause.select()

			# Laod test comds sequence from 'test_definition.py'
			btn_test=tk.Button(	frame_c2,text='Add test',		command= lambda: self.on_btn( Control.Action.Test ) )

			# check : exit test
			var=tk.IntVar()
			ch_exit=tk.Checkbutton( top,text='Exit',onvalue=1,offvalue=0,variable=var,command=self.gui.workflow.on_exit_cmds_queue)
			#self.ch_exit_test_val=var
			self.checks['exit']=var

			# btn : Reload seetings
			btn_settings=tk.Button(	frame_c1,text='Settings',	command= lambda: self.on_btn( Control.Action.Settings ) )
			btn_add=tk.Button(		frame_c2,text='Add',		command= lambda: self.on_btn( Control.Action.Add, Pos.End ) )
			btn_prepend=tk.Button(	frame_c2,text='Prepend',	command= lambda: self.on_btn( Control.Action.Add, Pos.Begin ) )
			btn_params=tk.Button(	frame_c2,text='Print pars',	command= lambda: self.on_btn( Control.Action.Params ) )
			btn_reset=tk.Button(	frame_c2,text='Reset',		command= lambda: self.on_btn( Control.Action.Reset ) )
			btn_clear_log=tk.Button(	frame_c1,text='Clear log',		command= lambda: self.on_btn( Control.Action.ClearLog ) )

			btn_errors=tk.Button(	frame_c1,text='Errors',		command= lambda: self.on_btn( Control.Action.Errors ) )
			btn_save_op=tk.Button(	frame_c1,text='Save op',	command= lambda: self.on_btn( Control.Action.Operator_Save ) )
			btn_load_op=tk.Button(	frame_c1,text='Load op',	command= lambda: self.on_btn( Control.Action.Operator_Load ) )
			
	
			#from PIL import ImageTk, Image
			#img = ImageTk.PhotoImage(Image.open(path))
			
			self.img_red = tk.PhotoImage(file=os.path.join(self.vars['dir'], 'red.gif'))
			for i in range( self.MAX_CMD ):
				tk.Button(	frame_c22,image=self.img_red, height=8.4, width=8.4, command= lambda i=i: self.on_btn( Control.Action.Remove, i ) ).pack( side=tk.TOP, fill=tk.X )

			self.img = tk.PhotoImage(file=os.path.join(self.vars['dir'], 'plus.gif'))
			for i in range( self.MAX_CMD ):
				tk.Button(	frame_c23,image=self.img, height=8.4, width=8.4, command= lambda i=i: self.on_btn( Control.Action.Insert, i ) ).pack( side=tk.TOP, fill=tk.X )
			
			var=tk.IntVar()
			ch_default_params=tk.Checkbutton( frame_c2,text='Default pars',onvalue=True,offvalue=False,variable=var)
			self.checks['default_params']=var
			ch_default_params.select()

			var=tk.IntVar()
			ch_run_one=tk.Checkbutton( frame_c2,text='Run one',onvalue=True,offvalue=False,variable=var)
			self.checks['run_one']=var
			ch_run_one.select()

			for btn in [ btn_settings, btn_add, btn_prepend, btn_params, btn_reset, btn_test, btn_clear_log, btn_errors, btn_save_op, btn_load_op ]: 
				btn.pack( side=tk.TOP, fill=tk.X )
			for check in [ ch_pause, ch_run_one, ch_default_params ]: 
				check.pack( anchor = tk.W )


			btn_errors.bind('<Enter>', lambda event: self.on_enter( Control.Action.Errors ) )
			btn_errors.bind('<Leave>', lambda event: self.on_leave( Control.Action.Errors ) )
			self.btns[ Control.Action.Errors ]=btn_errors

			#for btn in [ btn_errors_req ]: 
			#	btn.pack( side=tk.TOP, fill=tk.X )

			# --- Disabled
			#items = [ self.cb, btn_send, check ]
			items = []
			for item in items:
				item['state']=tk.DISABLED
			#self.cmds_queue['state']=tk.DISABLED # can't insert / delete from now on

			# -------------> C3 : below cmd's queue btns
			label_1 = tk.Label(	frame_c3,text="Remove: " )
			btn_1=tk.Button(	frame_c3,text='First',	command= lambda: self.on_btn( Control.Action.Remove_FIRST ) )
			btn_2=tk.Button(	frame_c3,text='Last',	command= lambda: self.on_btn( Control.Action.Remove_LAST ) )
			btn_3=tk.Button(	frame_c3,text='All',	command= lambda: self.on_btn( Control.Action.Remove_ALL ) )
			for btn in [ label_1, btn_1, btn_2, btn_3 ]: 
				btn.pack( side=tk.LEFT, fill=tk.X )

			# status
			self.labels['status']=tk.Label(top,text='STATUS BAR',anchor=tk.W,justify=tk.LEFT)

			# -------------> C5 : below done queue btns

			tk.Button( frame_c5, text='Clear', command= lambda: self.on_btn( Control.Action.Clear_DONE ) ).pack(	side=tk.LEFT )
			tk.Button( frame_c5, text='Save', command= lambda: self.on_btn( Control.Action.Save ) ).pack(	side=tk.LEFT )


			########### (b) Layout : grid ###########

			# title
			r=0
			label_R.grid(		row=r,	column=2 )
			label_A.grid(		row=r,	column=3 )
			label_queue.grid(	row=r,	column=4 )
			label_arrow.grid(	row=r,	column=5 )	# '->'
			label_done.grid(	row=r,	column=6 )
			label_sep.grid(		row=r,	column=8 )	# '-'
			label_pars_2.grid(	row=r,	column=9 )
			label_sep_2.grid(	row=r,	column=10 )	# '-'
			label_pars_done.grid(	row=r,	column=11 )

			r=1
			#self.doing.grid(		row=r, column=3)
			self.labels['doing'].grid(	row=r, column=4)
			self.total_time.grid(		row=r, column=7)

			# cmds_queue | cmds_done | elapsed_times
			r,r_span=(2,self.MAX_CMD)
			self.cmds_queue.grid(			row=r,column=4,   rowspan=r_span )
			self.cmds_done.grid(			row=r,column=6, rowspan=r_span )
			self.elapsed_time.grid(			row=r,column=7, rowspan=r_span )
			self.params_queue.grid(			row=r,column=9,  rowspan=r_span )
			self.params_queue_done.grid(	row=r,column=11,  rowspan=r_span )


			"""
			# combo
			r,c=(1,1)
			self.cb.grid(row=r,column=c)

			# params
			r,c=(2,0)
			for input in self.inputs: 
				input['label'].grid(	row=r,column=c )
				input['entry'].grid(	row=r,column=c+1 )
				r+=1
			"""

			# status
			#r,c,c_span=(8,0,3)
			#self.labels['status'].grid(row=r,column=c,columnspan=c_span)

			# Frames
			r_span=self.MAX_CMD
			#frame_c1.grid(	row=2,			column=1,rowspan=r_span, sticky=tk.N )		# inputs
			self.labels['status'].grid(row=2+r_span,column=0,columnspan=2)
			frame_c1.grid(	row=2,			column=0,rowspan=r_span, sticky=tk.N )		# btns stack at left side of cmds queue (First column)
			frame_c2.grid(	row=2,			column=1,rowspan=r_span, sticky=tk.N )		# btns stack at left side of cmds queue (Second column)
			frame_c22.grid(	row=2,			column=2,rowspan=r_span, sticky=tk.N )		# remove command btns
			frame_c23.grid(	row=2,			column=3,rowspan=r_span, sticky=tk.N )		# add command btns
			frame_c3.grid(	row=2+r_span,	column=4 )									# btns row below cmds queue
			frame_c5.grid(	row=2+r_span,	column=6 )									# btns row below done cmds queue	
			#frame_c6.grid(	row=r,column=c+4, rowspan=r_span ) # ch : Default params

			#label.place(relx=0.5, rely=0.5, anchor=tk.CENTER) # doesn't work
			#_center( toplevel ) # doesn't work
		except:
			helper.handle_exception()

	def bottom( self, bottom ):
		try:

			# a. pair : label + entry -> wg + layout
			r=0
			frame_entries = Frame( bottom )
			for key in self.entries_keys:			
				self.create_entry( frame_entries, key, r )	# ( parent, name, list )
				r+=1

			# b. radios:

			# 1. widgets
			for radio in self.radios_def:			
				self.create_radiobutton( bottom, radio[ 0 ], radio[ 1 ], radio[ 2 ], radio[ 3 ] )	# ( parent, name, list, parent frame )


			# 2. layout:
			frame_entries.grid( row=0,	column=0, sticky=tk.N )

			col=1
			for frame in self.frames:
				frame.grid( row=0, column=col, sticky=tk.N )
				col+=1
		
		except:
			helper.handle_exception()

	def radio_to_entry( self, name_radio ): # ATTENTION: 'name_radio' must be provided and NOT 'name_entry'

		try:
			value = Controls.value_rb( self.radios[ name_radio ]['var'] )

			if name_radio == GI.Depletion.Id.Name:
				if value == GI.Depletion.Id.Other:			value=0
				else:										value=-value-1
			elif name_radio == GI.Depletion.Output.Name:	value=value+1

			logging.debug('control : update_entry : radio_id: {} : value: {} )'.format( name_radio, value ) )
			#---------- # set radiobutton value to its corresponding entry
			name_entry=None
			if name_radio in self.equivalences_radios_entries: # write combo to entry
				name_entry = self.equivalences_radios_entries[ name_radio ]
				control_=self.entries[ name_entry ]
				#Controls.value( control_, str( value ) if value==0 else value ) # Take care with this !!!
				Controls.value_en( control_, str( value ) if value==0 else value ) # Take care with this !!!
			else:
				logging.error('control : update_entry : rb: {} : ERROR'.format( name_radio ) )
			logging.debug('control : update_entry : name_entry: {} )'.format( name_entry ) )
		except:
			helper.handle_exception()

	def update_colors( self ):

		try:
			for name_entry, entry in self.entries.iteritems(): #{ key (entry name): value (entry itself) }
				Controls.color( entry, 'red' if name_entry == 'cmd' else 'white' )

			cmd_id = Controls.value_rb( self.radios[ GI.Cmd.Name1 ]['var'] )
			cmds_len = len( GI.Cmd.params )
			#logging.info('len cmds: {}'.format( len_ ) )
			if cmd_id < cmds_len:
				params=GI.Cmd.params[ cmd_id ]
				for param in params:
					if param in self.entries:
						Controls.color( self.entries[ param ], 'green' )
					else:
						logging.error("control : update_colors : param: '{}' : NOT found in entries list".format( param ))
				Controls.value( self.labels['status'], '{} ({}x)'.format( params, len( params ) ) )
			else:
				Controls.value( self.labels['status'], 'ERROR' )
		except:
			helper.handle_exception()

	def create_params_dict( self ):

		try:
			all_params_dict=self.values()
			cmd_params_dict={}
			cmd_id = Controls.value_rb( self.radios[ GI.Cmd.Name1 ]['var'] )
			cmds_len = len( GI.Cmd.params )

			if cmd_id < cmds_len:

				params=GI.Cmd.params[ cmd_id ]
				for param in params:

					if param in self.entries:
						cmd_params_dict[ param ] = all_params_dict[ param ]
					else:
						logging.error("control : create_params_dict : param: '{}' : NOT found in entries list".format( param ))
			else:
				logging.error("control : create_params_dict : cmd_id: '{}' : params NOT defined".format( GI.Cmd.name[ cmd_id ] ))
			return cmd_params_dict
		except:
			helper.handle_exception()

	def show_result( self, text, time=2*1000 ):
		Controls.value( self.labels['status'], text )
		self.root.after( time, lambda: Controls.value( self.labels['status'], ' ' ) )

	def on_enter( self, btn_id, param=NOTHING ):

		try:
			#logging.debug('on_enter( {} )'.format( Control.Action.List[ btn_id ] ) )
			Controls.value( self.labels['status-top'], self.help[ btn_id ] )
		except:
			helper.handle_exception()

	def on_leave( self, btn_id, param=NOTHING ):

		try:
			#logging.debug('on_leave( {} )'.format( Control.Action.List[ btn_id ] ) )

			if btn_id == Control.Action.Errors:
				#Controls.value( self.labels['status'], '' )
				Controls.value( self.labels['status-top'], '' )
		except:
			helper.handle_exception()

	def on_btn( self, btn_id, param=NOTHING ):

		try:
			logging.debug('on_btn( {} )'.format( Control.Action.List[ btn_id ] ) )

			# cmds queue
			if btn_id == Control.Action.Other:
				pass

			elif btn_id == Control.Action.Remove_FIRST:

				self.gui.workflow.cmds_queue.remove( CmdsQueue.Remove.First )
				self.update_cmds_queue()

			elif btn_id == Control.Action.Remove_LAST:

				self.gui.workflow.cmds_queue.remove( CmdsQueue.Remove.Last )
				self.update_cmds_queue()

			elif btn_id == Control.Action.Remove_ALL:

				self.gui.workflow.cmds_queue.remove( CmdsQueue.Remove.All )
				self.update_cmds_queue()
				self.update_cmd_doing( GI.Cmd.NoCmd )

			# cmds done
			elif btn_id == Control.Action.Clear_DONE:

				self.cmds_done.delete('1.0', tk.END)
				self.elapsed_time.delete('1.0', tk.END)

			# others
			elif btn_id == Control.Action.Test:

				#self.gui.workflow.on_add_cmd( GI.Cmd.Operation.Test )

				import test_definition
				reload( test_definition )

				#logging.debug( 'workflow.on_add_cmd( {} )'.format( CmdsQueue.cmds_s( cmd_id if isinstance( cmd_id, list ) else [ cmd_id ] ) ) )

				for cmd in test_definition.cmds:

					params={}
					if isinstance(cmd, (int, long)):	#[ int, int, int ]
						cmd_id=cmd
					else:								# [ ( int,{ params } ) ]
						cmd_id=cmd[0]
						params=cmd[1]

					self.gui.workflow.on_add_cmd( cmd_id, Pos.End, params )

			elif btn_id == Control.Action.Pause:
				pause = Controls.value_ch( self.checks['pause'] )
				self.gui.workflow.cmds_queue.paused = pause
				self.gui.after( GI.Value.PollingInterval_Short, self.gui.workflow.cmds_queue.exe )

			elif btn_id == Control.Action.Settings:
				import settings1
				reload( settings1 )

				# matrix pairing settings with vars
				logging.getLogger().setLevel( settings1.settings1['loggging_level'] )
				self.gui.workflow.LINES		= settings1.settings1['LINES']
				self.gui.workflow.force_can = settings1.settings1['force_can']
				self.show_result( 'Settings updated...' )

			elif btn_id == Control.Action.Radio:

				name = param
				self.radio_to_entry( name )
				self.update_colors()

				if name == GI.Material.Model.Name:
					self.update_mat_model_params()

			elif btn_id == Control.Action.Add:

				cmd_id = Controls.value_rb( self.radios[ GI.Cmd.Name1 ]['var'] )

				pos = param

				if Controls.value_ch( self.checks['default_params'] ): #FIXME: checklist dict and access them by name, not access the var itself 
					self.gui.workflow.on_add_cmd( cmd_id, pos, {} )
				else:
					self.gui.workflow.on_add_cmd( cmd_id, pos, self.create_params_dict() )

			elif btn_id == Control.Action.Send: # NOT USED
				pass

			elif btn_id == Control.Action.Params:
				
				# Test1
				"""
				cmd_id = Controls.value_rb( self.radios[ GI.Cmd.Name1 ]['var'] )
				params=self.create_params_dict()
				logging.info('cmd: {}, params:{}'.format( cmd_id, params ))
				"""

				# Test2
				import pprint
				from material import models_def, params_def
				"""
				for model in models_def:
					pretty_dict_str = pprint.pformat( model )
					logging.info('\n{}'.format( pretty_dict_str ))
				"""
				pretty = pprint.pformat( models_def )
				logging.info('\n{}'.format( pretty ))

				pretty = pprint.pformat( params_def )
				logging.info('\n{}'.format( pretty ))

			elif btn_id == Control.Action.Reset:
				'''fills entries and sets colors'''

				#self.radios={} -> self.radios[ name ]	= { 'frame' : frame, 'var' : var }
				#self.entries	-> { key (entry name): value (entry itself) }

				# non-radios
				for entry_key in self.entries_keys_non_radio:
					Controls.value( self.entries[ entry_key ], self.default_values[ entry_key ] )

				# radios : (1) default value and (2) entries : radio -> entry
				for radio in self.radios_def: # [  [ name, list, default_value ], ]
					id = radio[0]
					if id not in [ GI.Cmd.Name2, GI.Cmd.Name3, GI.Cmd.Name4 ]:
						value_def = radio[2]
						Controls.value_rb( self.radios[ id ]['var'], str( value_def ) ) # Take care !!!
						self.radio_to_entry( id ) # name_radio : radio -> entry

				# bg color depending on the selected cmd
				self.update_colors()

			elif btn_id == Control.Action.Remove:
				'''removes cmd with index 'idx' from the cmds queue'''
				idx = param
				self.gui.workflow.cmds_queue.remove( CmdsQueue.Remove.Index, idx )
				self.update_cmds_queue()

			elif btn_id == Control.Action.Insert:
				'''inserts cmd with to a given position'''

				pos = param
				cmd_id = Controls.value_rb( self.radios[ GI.Cmd.Name1 ]['var'] )

				if Controls.value_ch( self.checks['default_params'] ): #FIXME: checklist dict and access them by name, not access the var itself 
					self.gui.workflow.on_add_cmd( cmd_id, pos, {} )
				else:
					self.gui.workflow.on_add_cmd( cmd_id, pos, self.create_params_dict() )

			elif btn_id == Control.Action.Save:
				#from collections import OrderedDict
				#map=OrderedDict()
				map=[]
				for cmd_id, params in zip( self.gui.workflow.cmds_queue.cmds_done, self.gui.workflow.cmds_queue.params_done):
					map.append( (GI.Cmd.name[ cmd_id ], params ) )

				import pprint
				pretty_dict_str = pprint.pformat( map )
				logging.info('control : save : \n{}'.format( pretty_dict_str ) )

				self.show_result( 'Save done...' )

				#import json
				#logging.info('control : save : \n{}'.format( json.dumps( map, indent=4, sort_keys=True) ) )

			elif btn_id == Control.Action.ClearLog:
				#open( self.gui.LOG_FILE, 'w').close()
				#with open( self.gui.LOG_FILE, 'w') as file:
				#	file.truncate()
				#	pass

				#from os import remove
				#remove( self.gui.LOG_FILE )

				# print GUI radiobuttons current value
				"""
				radios=[ 
					[ 'invert_for',	GI.InvertFor.List ],
					[ 'dep_type',	GI.Depletion.List ],
					[ 'dep_action',	GI.Depletion.Action.List ],
					[ 'plot',		GI.Plot.List ] ]

				for radio in radios:
					logging.info('control : gui : radios : {}: {}'.format( radio[0], radio[1][ Controls.value_rb( self.gui.radios[ radio[ 0] ] ) ]  ) )
				"""

				# FIXME: do it better, part of this code it's already done in the GUI file
				try:
					f = open( self.gui.LOG_FILE )
					timestamp = f.readline()[0:16].replace(':','-').replace(' ','_')
					dst = '{}_{}.log'.format( self.gui.LOG_FILE[0:-4], timestamp )
				except:
					pass
				LOGGING_LEVEL_DICT={ logging.CRITICAL : 'CRITICAL', logging.ERROR : 'ERROR', logging.WARNING : 'WARNING', logging.INFO : 'INFO', logging.DEBUG : 'DEBUG' }

				# Remove all handlers associated with the root logger object.
				for handler in logging.root.handlers[:]:
					logging.root.removeHandler(handler)

				import settings1
				reload( settings1 )

				logging.basicConfig( filename= self.gui.LOG_FILE, format='%(asctime)s %(message)s', level=settings1.settings1['loggging_level'], filemode='w' )
				if dst:
					logging.info('previous execution: {}'.format( dst ) )
				logging.info('current logging level: {}'.format( LOGGING_LEVEL_DICT[ logging.getLogger().getEffectiveLevel() ] ) )

				self.show_result( 'Log file cleared...' )

			elif btn_id == Control.Action.Errors:
				cnt=0
				with open( self.gui.LOG_FILE ) as fp:  
					line = fp.readline()
					if 'Exception' in line: cnt+=1
					while line:
						line = fp.readline()
						if 'Exception' in line: cnt+=1
				self.show_result( '{}'.format( 'Execution OK' if cnt==0 else ('ERRORS: '+cnt) ) )

			elif btn_id == Control.Action.Operator_Save:
				# to binary file
				FILE_A='E:\IT_A.dat'
				FILE_A2='E:\IT_A2.dat'
				FILE_scale2='E:\IT_scale2.dat'
				FILE_scale_disp='E:\IT_scale_disp.dat'
				logging.info('control : save : A: {} : type: {} : shape: {}'.format( self.gui.A, type( self.gui.A ), self.gui.A.shape ) )
				logging.info('control : save : A2: {} : type: {} : shape: {}'.format( self.gui.A2, type( self.gui.A2 ), self.gui.A2.shape ) )
				logging.info('control : save : scale2: {} : type: {} : shape: {}'.format( self.gui.scale2, type( self.gui.scale2 ), self.gui.scale2.shape ) )
				logging.info('control : save : scale_disp: {} : type: {} : shape: {}'.format( self.gui.scale_disp, type( self.gui.scale_disp ), self.gui.scale_disp.shape ) )
				#self.shape=self.gui.A.shape
				self.gui.A.tofile( FILE_A )
				self.gui.A2.tofile( FILE_A2 )
				self.gui.scale2.tofile( FILE_scale2 )
				self.gui.scale_disp.tofile( FILE_scale_disp )
				# to text file
				'''
				import numpy as np
				FILE_A_TXT='E:\IT_A.txt'
				FILE_A2_TXT='E:\IT_A2.txt'
				np.savetxt( FILE_A_TXT, self.gui.A ) #fmt='%d'
				np.savetxt( FILE_A2_TXT, self.gui.A2 )
				'''
			elif btn_id == Control.Action.Operator_Load:
				# from binary file
				import numpy as np
				FILE_A='E:\IT_A.dat'
				FILE_A2='E:\IT_A2.dat'

				self.gui.A = np.ndarray( shape=self.gui.A.shape, buffer=np.fromfile( FILE_A ) ) #(file, dtype=float, count=-1, sep='')
				self.gui.A2 = np.ndarray( shape=self.gui.A2.shape, buffer=np.fromfile( FILE_A2 ) ) #(file, dtype=float, count=-1, sep='')
				logging.info('control : load : A: {}'.format( self.gui.A ) )
				logging.info('control : load : A2: {}'.format( self.gui.A2 ) )

		except:
			helper.handle_exception()

	def on_cmds_new_selection( self, event ):
		try:
			caller = event.widget
			if caller == self.cb:

				cmd_id = Controls.value_cb( self.cb )

				self.update_params_labels( cmd_id )
				self.set_params_default_values( cmd_id )

				Controls.value( self.labels['status'], GI.Cmd.params[ cmd_id ] )
		except:
			helper.handle_exception()

"""
def main():
	s=Sender( None )

if __name__=='__main__':
	main()
"""
