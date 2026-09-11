import ttk # Progress bar
import Tkinter as tk # tk.CENTER

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
