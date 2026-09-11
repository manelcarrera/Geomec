from geomec_interface import GeomecInterface as GI
import threading
import logging
import helper #handle_exception

'''
http://stackoverflow.com/questions/16745507/tkinter-how-to-use-threads-to-preventing-main-event-loop-from-freezing
'''

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
