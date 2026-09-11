import logging
import helper
import Queue
from geomec_interface import GeomecInterface as GI # GI.Cmd
from progress import Progress

class Commands():

	def __init__( self, gui ):

		try:
			self.gui = gui

			self.progress = Progress( gui )
		except:
			helper.handle_exception()

	def send( self, cmd_list ):

		try:

			logging.debug('Commands : send : cmd_list:{}'.format( cmd_list ) )

			IniCmd = ( GI.Cmd.Control.Ini, () )
			EndCmd = ( GI.Cmd.Control.End, () )

			cmds = [ IniCmd ] + cmd_list + [ EndCmd ]

			logging.debug('-->')
			self.print_( cmd_list )

			self.progress.show()

			self.queue = Queue.Queue() # ???? new queu each time?

			num_cmd = len( cmds )
			for i in range( num_cmd ):
				cmd=cmds[ i ]
				#logging.debug('cmd:{}'.format( cmd ) )
				ThreadedTask( self.queue, cmd ).start()
				sleep(0.2) # Time in seconds. #FIXME

			self.gui.after( GI.Value.PollingInterval, self.on_receive )

		except:
			helper.handle_exception()

	def on_receive(self):
		'''
		This emethod is called every 'GI.Value.PollingInterval' time to check if the response to a request is available.
		Default time: 100ms.
		When respnse received it's treated in 'ResultsHandler' class in 'results_handler.py' module
		'''
		try:
			res = self.queue.get(0) # res: tuple( obj1, obj2, ..., objn )
									# obj1 = cmd = (cmd_id,param)

			try:
				self.is_processing_task = False
				#logging.debug('process_queue : res : {}'.format(res))
				self.print_received( res )
				res1=self.gui.workflow.on_result( res )
			
				#if self.gui.workflow.current_cmd != GI.Cmd.IT1.Get.Reservoir: # before that it needs to (1) assign vals to vars and (2) insert GI.Cmd.Operation.Reservoir to be run
				self.gui.workflow.post( res1 )
				#self.gui.workflow.current_cmd=GI.Cmd.NoCmd
				
				self.progress.hide()

			except:
				helper.handle_exception()

		except Queue.Empty:
			self.gui.after( GI.Value.PollingInterval, self.on_receive )

	#FIXME: It doesn't work fine for LISTs and 'ok'/'nok' responses 
	def print_received(self, res): # ( (cmd_id,data), (cmd_id,data),... )
		try:
			n=0
			size=len(res)
			logging.debug('received:')# size:{}'.format(size))
			for elem in res:

				if n == 0 and size == 1:
					cmd_id, data = elem
					if cmd_id in [	GI.Cmd.IT1.Set.Depletion, 
									GI.Cmd.IT1.Exe.Run_Geomec, 
									GI.Cmd.IT1.Exe.Run_Geomec_Step, 
									GI.Cmd.IT1.Exe.Wait_End_Run_Geomec, 
									GI.Cmd.IT1.Exe.Remove_Depletion,
									
									GI.Cmd.IT1.List.Reservoir,
									GI.Cmd.IT1.List.Surface,
									GI.Cmd.IT1.List.Depletion,
									GI.Cmd.IT1.List.Measured_Displacement,

									GI.Cmd.Gen.List ]:

						logging.debug( '{}'.format( data ) )
						break

				e = elem[ 1 ][ 0 ] if len( elem[1] ) == 1 else elem[ 1 ]
				logging.info(' n:{} : ({}x) {}'.format( n, len( e ), e[0:self.gui.workflow.LINES] ) )
				n+=1
		except:
			helper.handle_exception()

	def print_( self, cmd_list ): # FIXME: code duplicate

		try:
			cmd_s = '[ '
			for cmd in cmd_list:
				cmd_s += (GI.Cmd.name[ cmd[0] ] + ', ')
			cmd_s = cmd_s[:-2]
			cmd_s += ' ]'
			logging.debug('{}'.format( cmd_s ))
			#logging.debug('{}'.format( cmd_list ) )

		except:
			helper.handle_exception()
