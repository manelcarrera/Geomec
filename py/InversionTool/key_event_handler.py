import helper
import logging

from geomec_interface import GeomecInterface as GI
from geomec_interface_ext import GeomecInterfaceExt as GIE
#from control import Sender
from interface import Commands

class KeyEventHandler():

	DEFAULT_RESERVOIR_FILE		= 'C:\\dev\\example_dataset\\top_reservoir.ts'
	DEFAULT_DISPLACEMENT_FILE	= 'E:\\Inversion\\GeomecPlugin\\TESTDATA\\groningen_disp_simple.txt'
	DEFAULT_DISPLACEMENT_FILE	= 'E:\\Inversion\\GeomecPlugin\\TESTDATA\\silvertip_disp_test.txt'
	#DEFAULT_DISPLACEMENT_FILE	= 'E:\\Inversion\\GeomecPlugin\\TESTDATA\\disc.txt'

	#def __init__( self, gui ):
	#	self.gui = gui

	@staticmethod
	def send_cmd( gui, cmd_id ):
		commands = Commands( gui )
		commands.send( [ commands.build( cmd_id ) ] )

	@staticmethod
	def handle( gui, e ): #event

		try:
			#print ('key: char:{} code:{} keysym:{}'.format(e.char,e.keycode,e.keysym) )
			if e.char == e.keysym:
				pass
				#msg = 'Normal Key %r' % event.char
			elif len(e.char) == 1:
				pass
				#msg = 'Punctuation Key %r (%r)' % (event.keysym, event.char)
			else:
				#msg = 'Special Key %r' % event.keysym
		
				#if e.char > '1':
				#	print('uno')

				logging.debug( e.keysym )

				if e.keysym == "F1": #reservoir

					logging.debug('-F1-')
					from reservoir import Reservoir
					filename=KeyEventHandler.DEFAULT_RESERVOIR_FILE
					#gui.reservoirpoints,gui.reservoirtriangles,gui.reservoirpars,gui.reservoirheader=Reservoir.get( filename )
					#gui.reservoir()
					gui.results_handler.handle( (GI.Cmd.IT1.Reservoir, Reservoir.get( filename ) ) )

				elif e.keysym == "F2": #displacement

					logging.debug('-F2-')
					from displacement import Displacement
					filename=KeyEventHandler.DEFAULT_DISPLACEMENT_FILE
					#gui.yirr,gui.xirr,gui.zirr,gui.wdat,gui.subs,gui.header = Displacement.transform( Displacement.get( filename ) )
					#displacement = Displacement.get( filename )
					#logging.debug('displacement_from_file:{}'.format(displacement))
					#gui.yirr,gui.xirr,gui.zirr,gui.wdat,gui.subs,gui.header = displacement
					gui.yirr,gui.xirr,gui.zirr,gui.wdat,gui.subs,gui.header = Displacement.get( filename )
					gui.subsidence()
					#gui.results_handler.handle( (GeomecInterface.Request_Id.Cmd_Get_Displacement, Displacement.get( filename ) ) )

				elif e.keysym == "F3":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Set.Depletion )
				elif e.keysym == "F4":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Exe.Run_Geomec )
				elif e.keysym == "F5":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Exe.Remove_Depletion )
				#elif e.keysym == "F6":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Get.Material_Parameter )
				elif e.keysym == "F6":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Get.Measured_Displacement )
				elif e.keysym == "F7":	KeyEventHandler.send_cmd( gui, GI.Cmd.IT1.Get.Displacement )
				elif e.keysym == "F8":	
					if gui.control:
						gui.control.show_toggle()
					#else:
					#	gui.control=Sender( gui )

				elif e.keysym == "F9":
					logging.debug('-F9-')
					#gui.progress()
					#from Tkinter import ttk
					import Tkinter              # Python 2
					import ttk
					#toplevel = gui.Toplevel()
					gui.prog_bar = ttk.Progressbar( gui, orient="horizontal",	length=200, mode="indeterminate" )
					#gui.prog_bar.pack( expand=True, fill=Tkinter.BOTH, side=Tkinter.TOP )
					gui.prog_bar.place(relx=0.5, rely=0.5, anchor=Tkinter.CENTER)
					gui.prog_bar.start()

				elif e.keysym == "F11":
					logging.debug('-F11-')
					gui.prog_bar.stop()
					gui.prog_bar.destroy()


				# Geomec Interface (EXTENDED)
				# This doesn't work -> analyze why
				'''
				elif e.keysym == "F9":
					logging.debug('-F9-')
					gui.send_req( GeomecInterfaceExt.Request_Id.Cmd_Get_Reservoir_From_File )
				elif e.keysym == "F10":
					logging.debug('-F10-')
					gui.send_req( GeomecInterfaceExt.Request_Id.Cmd_Get_Displacement_From_File )
				elif e.keysym == "F11":
					logging.debug('-F11-')
					gui.send_req( GeomecInterfaceExt.Request_Id.Cmd_Get_Generic )
				'''
		except:
			helper.handle_exception()
