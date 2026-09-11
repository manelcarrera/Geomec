import logging
import helper
import Tkinter as tk # tk.CENTER
import tkFont #underline

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
