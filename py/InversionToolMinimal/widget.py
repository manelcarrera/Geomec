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
			[ 'internal iterations',					'5',	'#']
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

			btn1= tk.Button( frame_btns,text='Reload data', command= lambda: self.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir,Pos.End,{} ) )

			for btn in [ btn1 ]:
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
			label.grid(	row=r, column=c )
			combo.grid(	row=r, column=c+1, columnspan=2, padx=5 )

			self.gui.combos['res']=combo
			#self.gui.combos['res'].bind("<<ComboboxSelected>>", self.gui.on_cb_update)
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
