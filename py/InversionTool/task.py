import logging
import Queue
from geomec_interface import GeomecInterface as GI # GI.Cmd
from interface import Progress
import helper

import threading

#import sys

class ThreadedTaskExe(threading.Thread):

	def __init__(self, queue, task ):

		threading.Thread.__init__(self)
		self.queue = queue
		self.task=task

	def run(self):

		try:
			res=self.task()
			self.queue.put( res )
		except:
			helper.handle_exception()


class Task():

	def __init__( self, gui ):
		self.gui=gui
		self.progress = Progress( gui )

	def exe( self, task ):

		try:
			#logging.debug('--> Task.exe( {} )'.format( task.__name__ ) )
			#logging.debug('sys._getframe().f_code.co_name: {}'.format( sys._getframe().f_code.co_name ) )
			self.progress.show()
			self.queue = Queue.Queue()

			ThreadedTaskExe( self.queue, task ).start()
			self.gui.after( GI.Value.PollingInterval_Long, self.end )
		except:
			helper.handle_exception()

	def end(self):
		'''tests if operation is done and if sohandles GUI after having run an operation, otherwise it sets on the trigger to check it out again later'''

		try:
			res = self.queue.get(0)

			try:
				self.gui.workflow.post( True )
				self.progress.hide()

				#self.gui.workflow.current_cmd = GI.Cmd.NoCmd # to go to next command in test batch process

				logging.debug('<-- End')
			except:
				helper.handle_exception()
		except Queue.Empty:
			self.gui.after( GI.Value.PollingInterval_Long, self.end )
