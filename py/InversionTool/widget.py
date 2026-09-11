from geomec_interface import GeomecInterface as GI # Cmd
import helper
import logging

import ttk #combobox
import Tkinter as tk
from Tkinter import Frame
from interface import Controls
import tkFont #underline
from cmds_queue import Pos

class Widget:

	def __init__(self, gui, workflow ):
		self.gui=gui
		self.workflow = workflow

		self.params_def=[ 
			[ 'internal iterations',					'5',	'#'],			# 0
			[ 'external iterations',					'1',	'#'],			# 1
			[ 'reweight iterations\noutlier detection', '5',	'#'],			# 2
			[ 'reduction factor',						'10',	'%'],			# 3
			[ 'Subsidence sign',						'+',	'+ or -']		# 4
		]

	def getinput(self):
		'''Obtain the inversion setting values from the input fields.'''
		try:
			self.gui.inputs={} #map

			n=0
			for param in self.params_def:
				key = param[ 0 ]
				value = self.gui.params_wg[ n ].get()
				self.gui.inputs[ key ] = value
				n+=1

		except:
			helper.handle_exception()

	"""
		Layout:
		------------------------------------------------------------------------------------------
		0					1			2				3					4

		0	Input1-Name		I1-Entry	I1-Units		-					Plot (row 0 - row 11?)
			...
		4	I5-Name			I5-Entry	I5-Units		-

		5	Invert-for		Combo						-
		6	Depletion		Checkbox	Checkbox		-

		7	Btn-Const		Btn-Invert	Btn-Set_dep		Btn-Run_Geomec

		8	Reservoir		Combo		Fill			Get
		9	Surface			Combo		Fill			
		10	Plot			Combo
		11	Meas disp		Combo		Fill			Get		
		10	Output			Combo
		11	Model disp									Get		

		12	Message

		13	Status			cmd_id		Btn-Test		Checkbox-Exit_test

	"""
	def create(self):

		try:

			# ================== Entries ================== (5x) row = 0..4
			r,c=(0,0)
			self.gui.params_wg=[]
			for param in self.params_def:

				label=tk.Label(self.gui,text=param[0],width=15,anchor=tk.W,justify=tk.LEFT)

				entry=tk.Entry(self.gui,width=10)
				entry.insert(0,param[1])

				label2=tk.Label(self.gui,text=param[2],width=5,anchor=tk.W,justify=tk.LEFT)

				label.grid(  row=r, column=0, padx=5 )
				entry.grid(	 row=r, column=1 )
				label2.grid( row=r, column=2 )

				self.gui.params_wg.append( entry )

				r+=1

			# ================== Buttons ================== row = 7

			frame_btns = Frame( self.gui )

			#btn1=tk.Button( frame_btns,text='Construct',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.Operation.Construct, Pos.End, {} ))
			#btn1= tk.Button( frame_btns,text='Reload data', command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir,Pos.End,{} ) )
			btn2=tk.Button( frame_btns,text='Invert',		command= lambda: self.workflow.on_add_cmd( GI.Cmd.Operation.Invert, Pos.End, {} ))
			btn3=tk.Button( frame_btns,text='Set depletion', command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.Set.Depletion, Pos.End, {} ))
			btn4=tk.Button( frame_btns,text='Run GEOMEC',command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.Exe.Run_Geomec, Pos.End, {} ))


			for btn in [  btn2, btn3, btn4 ]:
				btn.pack( side=tk.LEFT, fill=tk.BOTH, expand=True, padx=10, pady=10 )

			frame_btns.grid(row=r,column=c, columnspan=3)

			#self.gui.b_construct=btn1
			self.gui.b_execute=btn2
			self.gui.b_setdep=btn3
			self.gui.b_rungm=btn4

			btns = [ self.gui.b_execute, self.gui.b_setdep, self.gui.b_rungm ]
			for btn in btns:
				btn['state'] = 'disabled'

			r+=1

			# ================== COMBOS + RADIOS ================== (6x) row = 8..13

			# (1) ----------------> reservoir
			label	= tk.Label(		self.gui,text='Reservoir:')
			combo	= ttk.Combobox( self.gui ) #textvariable=val
			#combo['values'] = ('X', 'Y', 'Z')
			#combo.current(0)
			#btn		= tk.Button(	self.gui,text='Fill',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir,Pos.End,{} ) )
			#btn2	= tk.Button(	self.gui,text='Get',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.Get.Reservoir,Pos.End,{} ) )
			#btn3	= tk.Button(	self.gui,text='Reload',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir,Pos.Ini,{} ) )

			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			#btn.grid(	row=r, column=c+2 )
			#btn2.grid(	row=r, column=c+3 )
			#btn3.grid(	row=r, column=c+2, columnspan=2 )

			self.gui.combos['res']=combo
			#self.gui.combos['res'].bind("<<ComboboxSelected>>", self.gui.on_cb_update)
			r+=1

			# (2) ----------------> surface: to select the top surface
			label	= tk.Label(		self.gui,text='Surface:')
			combo	= ttk.Combobox( self.gui ) #, textvariable=val
			#btn		= tk.Button(	self.gui, text='Fill',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Surface,Pos.End,{} ) )

			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			#btn.grid(	row=r, column=c+2 )

			self.gui.combos['sur']=combo
			r+=1

			# (3) ----------------> measured displacement
			label	= tk.Label(		self.gui,text='Meas. disp.:')
			combo	= ttk.Combobox( self.gui )
			#btn		= tk.Button(	self.gui, text='Fill',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Measured_Displacement,Pos.End,{} ) )
			#btn2	= tk.Button(	self.gui, text='Get',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.Get.Measured_Displacement,Pos.End,{} ) )

			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			#btn.grid(	row=r, column=c+2 )
			#btn2.grid(	row=r, column=c+3 )

			self.gui.combos['composite_idx']=combo
			#self.gui.combos['composite_idx'].bind("<<ComboboxSelected>>", GI.Cmd.IT1.Get.Measured_Displacement)
			r+=1

			# (4) ----------------> modelled displacement
			
			#label	= tk.Label(		self.gui,text='Model disp.:')
			#btn		= tk.Button(	self.gui, text='Get',	command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.Get.Displacement,Pos.End,{} ) )

			#label.grid(	row=r, column=c, columnspan=2 )
			#btn.grid(	row=r, column=c+1 )
			#r+=1
			

			# ================== Invert for ================== row = 5


			"""
			frame=Frame( self.gui )
			self.gui.labels['invert_for']=tk.Label( frame, text='Invert for:' )
			self.gui.labels['invert_for'].pack( side=tk.LEFT )
			var = tk.StringVar()
			idx=0
			for item in GI.InvertFor.List:
				tk.Radiobutton( frame, text=item, variable=var, value=idx ).pack( side=tk.LEFT )
				idx+=1
			self.gui.radios['invert_for']=var
			Controls.value_rb( self.gui.radios['invert_for'], GI.InvertFor.Strain )

			frame.grid(	row=r, column=0, columnspan=4 )
			"""

			label=tk.Label(self.gui,text="Invert for:")
			combo = ttk.Combobox( self.gui )
			label.grid(	row=r, column=c )
			combo.grid( row=r, column=c+1, columnspan=2, padx=5 )
			self.gui.combos['invert_for']=combo

			r+=1


			# (5) ----------------> output : pore pressure / strain / compressibility

			"""
			frame=Frame( self.gui )
			self.gui.labels['dep_type']=tk.Label( frame, text='Output:' )
			self.gui.labels['dep_type'].pack( side=tk.LEFT )
			var = tk.StringVar()
			idx=0
			for item in GI.Depletion.List_Short:
				tk.Radiobutton( frame, text=item, variable=var, value=idx, command= lambda idx=idx: self.gui.workflow.on_btn( GI.GUI.DepType, idx ) ).pack( side=tk.LEFT )
				idx+=1
			self.gui.radios['dep_type']=var
			Controls.value_rb( self.gui.radios['dep_type'], GI.Depletion.Strain )

			frame.grid(	row=r, column=0, columnspan=4 ) # sticky=tk.W
			"""

			label = tk.Label( self.gui,text='Output:')
			combo = ttk.Combobox( self.gui )
			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			self.gui.combos['dep_type']=combo

			r+=1

			# ================== depletion : new and replace ================== row = 6

			"""
			frame=Frame( self.gui )
			self.gui.labels['dep_action']=tk.Label( frame, text='Depletion:' )
			self.gui.labels['dep_action'].pack( side=tk.LEFT )
			var = tk.StringVar()
			idx=0
			for item in GI.Depletion.Action.List:
				tk.Radiobutton( frame, text=item, variable=var, value=idx ).pack( side=tk.LEFT )
				idx+=1
			self.gui.radios['dep_action']=var
			Controls.value_rb( self.gui.radios['dep_action'], GI.Depletion.Strain )

			frame.grid( row=r, column=0, columnspan=4)
			"""

			combo = ttk.Combobox( self.gui )
			label = tk.Label(self.gui,text="Depletion:")
			label.grid( row=r, column=c)
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			self.gui.combos['dep_action']=combo

			r+=1


			#  (6)  ----------------> Plot reservoir property

			"""
			frame=Frame( self.gui )
			self.gui.labels['plot']=tk.Label( frame, text='Plot:' )
			self.gui.labels['plot'].pack( side=tk.LEFT )
			var = tk.StringVar()
			idx=0
			for item in GI.Plot.List_Short:
				tk.Radiobutton( frame, text=item, variable=var, value=idx, command= lambda idx=idx: self.gui.workflow.on_btn( GI.GUI.Plot, idx ) ).pack( side=tk.LEFT )
				idx+=1
			self.gui.radios['plot']=var
			Controls.value_rb( self.gui.radios['plot'], GI.Plot.Depth )

			frame.grid(	row=r, column=0, columnspan=4 ) #sticky=tk.W
			"""

			label	= tk.Label(	self.gui,text='Plot:')
			combo	= ttk.Combobox( self.gui )
			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )
			self.gui.combos['plot']=combo

			r+=1

			for label in self.gui.labels.itervalues(): # just radiobuttons labels
				Controls.underline( label )

			# combos : common actions
			#combos_id = [ 'invert_for', 'res', 'sur', 'plot', 'composite_idx', 'dep_type' ]
			combos_id = [ 'res', 'sur', 'composite_idx', 'plot', 'dep_type']
			for combo_id in combos_id:
				combo=self.gui.combos[ combo_id ]
				combo['state'] = 'active'
				combo.bind("<<ComboboxSelected>>", self.workflow.on_combo_new_selection)

			#combos = [ self.gui.invert_for, self.gui.cb_show, self.gui.cb_type  ]
			#for combo in combos:
			#	combo.current(0)

			combos = [	('invert_for',	['Strain','Compaction'], GI.InvertFor.Strain ),
						('plot',		['Depth [m]','Thickness [m]','Poisson\'s ratio [-]','Young\'s modulus [GPa]','Initial pore pressure [MPa]','Compressibility [1/MPa]'], GI.Plot.Depth ), 
						('dep_type',	['Strain','Compressibility','Pore pressure','Temperature'], GI.Depletion.Strain ),
						('dep_action',  ['Current','Replace','New'], GI.Depletion.Action.Current ) ]
			for item in combos:
				combo_id,values,def_value=item
				combo=self.gui.combos[ combo_id ]
				combo['values'] = values
				combo.current( def_value )
			

			# ================== Messages ==================
			text=tk.Text(self.gui,relief=tk.SUNKEN,font=("Helvetica", 8),height=12,width=8)
			
			text.tag_configure('INFO',							font='helvetica 8 bold', relief='raised')
			text.tag_configure('OK',			font='helvetica 8 bold', relief='raised')
			text.tag_configure('WARN',	background='yellow',	font='helvetica 8 bold', relief='raised')
			text.tag_configure('ERROR', background='red',		font='helvetica 8 bold', relief='raised')

			text.insert(1.0,'Step 1: Select the reservoir\n','INFO')
			text.insert(1.0,'Step 2: Select the top reservoir surface\n','INFO')

			r_span,c_span=(1,3)
			text.grid(row=r,column=c,rowspan=r_span, columnspan=c_span,sticky=tk.W+tk.N+tk.E, padx=5)

			self.gui.textU = text
			r+=r_span

			return r
		except:
			helper.handle_exception()
