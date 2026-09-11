import os;
LOG_FILE=os.path.dirname(os.path.abspath(__file__)) + "\..\IT_py.log"

try:
	from Tkinter import Frame
	import Tkinter as tk

	# Imports for the Geomec interface
	import logging
	import helper
	from workflow import Workflow
	from geomec_interface import GeomecInterface as GI
	from widget import Widget
	from control import Sender
	import settings1
	from controls import Controls		

	try:
		f = open( LOG_FILE )
		timestamp = f.readline()[0:16].replace(':','-').replace(' ','_')
		dst = '{}_{}.log'.format( LOG_FILE[0:-4], timestamp )
		copyfile( LOG_FILE, dst )
	except:
		pass
	logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG, filemode='w' )
	logging.getLogger().setLevel( settings1.settings1['loggging_level'] ) # getLogger('logger_id_string')
	logging.info('previous execution: {}'.format( dst ) )
	LOGGING_LEVEL_DICT={ logging.CRITICAL : 'CRITICAL', logging.ERROR : 'ERROR', logging.WARNING : 'WARNING', logging.INFO : 'INFO', logging.DEBUG : 'DEBUG' }
	logging.info('current logging level: {}'.format( LOGGING_LEVEL_DICT[ settings1.settings1['loggging_level'] ] ) )
	#logging.info('creation:{}'.format( creation ) )
except:
	import logging
	logging.basicConfig( filename=LOG_FILE, format='%(asctime)s %(message)s', level=logging.DEBUG )
	import helper
	helper.handle_exception()


class gui(Frame):
	def __init__(self,parent=None):

		try:
			Frame.__init__(self,parent) # FIXME: don't do the work when app is launched sevreral times; tkk app needs to be closed properly

			self.combos={}
			self.checks={}
			self.radios={}
			self.labels={}

			self.workflow = Workflow( self )
			self.widget = Widget( self, self.workflow ) 
	
			rij = self.widget.create() # 'invert for' | 'New depletion' | Buttons | Combos

			control_wnd = settings1.settings1['control_wnd']
			if control_wnd:
				self.control=Sender( self ) # IMPORTANT
			else:
				self.control=None

		except:
			helper.handle_exception()

def main():

	try:
		root = tk.Tk()
		w = gui( root )

		root.bind("<KeyRelease>", w.workflow.on_key_release)

		//w.workflow.on_add_cmd( GI.Cmd.IT1.List.Reservoir )

		w.mainloop()  
		w.quit()
		root.quit()

	except:
		helper.handle_exception()

if __name__=='__main__':

	main()
