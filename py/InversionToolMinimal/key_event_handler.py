import helper
import logging

from geomec_interface import GeomecInterface as GI
#from control import Sender
from interface import Commands

class KeyEventHandler():

	@staticmethod
	def send_cmd( gui, cmd_id ):
		commands = Commands( gui )
		commands.send( [ commands.build( cmd_id ) ] )

	@staticmethod
	def handle( gui, e ): #event

		try:
			if e.char == e.keysym:
				pass
			elif len(e.char) == 1:
				pass
			else:
				logging.debug( e.keysym )
				if e.keysym == "F1": #reservoir
					pass
				elif e.keysym == "F8":	
					if gui.control:
						gui.control.show_toggle()

		except:
			helper.handle_exception()
