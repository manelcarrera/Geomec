import logging
from geomec_interface import GeomecInterface as GI # GI.Cmd
import helper
from time import sleep, time #used in send() and class 'Test'
from datetime import timedelta
from interface import Commands, NOTHING

class CmdsQueueContext():

	def __init__( self, cmds_queue ):
		self.cmds_queue=cmds_queue
		self.cmds=[]
		self.params=[]
		self.current_cmd=GI.Cmd.NoCmd

	def save( self ):
		self.cmds=self.cmds_queue.cmds
		self.params=self.cmds_queue.params
		self.current_cmd=self.cmds_queue.gui.workflow.current_cmd

	def restore( self ):
		self.cmds_queue.cmds=self.cmds
		self.cmds_queue.params=self.params
		self.cmds_queue.gui.workflow.current_cmd=self.current_cmd
		self.clear()

	def clear( self ):
		self.cmds=[]
		self.params=[]
		self.current_cmd=GI.Cmd.NoCmd

class Pos:
	Begin=0
	End=-1

class CmdsQueue():

	class Remove:
		All		= 0
		Last	= 1
		First	= 2
		Index	= 3
		List	= [ 'All', 'Last', 'First', 'Index' ]

	def __init__( self, gui ):
		try:
			self.gui=gui
			self.cmds=[] 
			self.params=[] #list of dicts
			self.params_doing={}
			self.params_done=[]
			self.cmds_done=[] 
			self.times=[] 
			self.commands = Commands( gui )
			self.context=CmdsQueueContext( self )
			self.gui.ch_exit_test_val=0
			self.paused = False
		except:
			helper.handle_exception()

	@staticmethod
	def isNaN(num):
		return num != num
	
	def remove( self, type, idx=NOTHING ):

		#logging.debug('cmds queue : remove : {}'.format( CmdsQueue.Remove.List[ type ] ) )

		if type == CmdsQueue.Remove.All:
			self.cmds=[]
			self.params=[]
		elif type == CmdsQueue.Remove.Last:
			if len( self.cmds ):
				self.cmds=self.cmds[0:-1]
				self.params=self.params[0:-1]
		elif type == CmdsQueue.Remove.First:
			if len( self.cmds ):
				self.cmds.pop( 0 )
				self.params.pop( 0 )
		elif type == CmdsQueue.Remove.Index:
			if idx < len( self.cmds ):
				self.cmds.pop( idx )
				self.params.pop( idx )

	def add( self, cmd, pos=Pos.End, params_dict={} ): # FIXME: params | only one command per time (not a list of commands as before)
		try:
			#cmds_list = cmds if isinstance( cmds, list ) else [ cmds ]
			if pos==Pos.End:
				#self.cmds.extend( cmds if isinstance( cmds, list ) else [ cmds ] )
				self.cmds.append( cmd )
				self.params.append( params_dict )
			elif pos==Pos.Begin: # TODO: verify
				self.cmds.insert(	0, cmd )
				self.params.insert( 0, params_dict )
			else:
				if pos < len( self.cmds ):
					self.cmds.insert( pos, cmd )
					self.params.insert( pos, params_dict )
				else:
					self.cmds.append( cmd )
					self.params.append( params_dict )

			if self.gui.control:
				self.gui.control.update_cmds_queue() # including params queue

			'''
			if isinstance( cmds, list ):
				self.cmds.extend( cmds )
			else:
				self.cmds.append( cmds )
			'''

			logging.debug('queue : add : cmd added: {} : params: {}'.format( GI.Cmd.name[ cmd ], params_dict ))
			logging.debug('queue : add : cmds all: {}'.format( CmdsQueue.cmds_s( self.cmds ) ))
			logging.debug('queue : add : params all cmds: {}'.format( self.params ))

			if self.gui.workflow.current_cmd == GI.Cmd.NoCmd and not self.paused:
				self.exe()
		except:
			helper.handle_exception()

	def exe( self ):

		try:
			if len( self.cmds ):
				if self.paused:
					#self.gui.after( GI.Value.PollingInterval_Long, self.exe )
					pass
				else:
					self.cmd=self.cmds.pop( 0 )
					params=self.params.pop( 0 )

					self.gui.workflow.current_cmd = self.cmd # IMPORTANT !!!!!

					logging.debug(' -> cmds queue : cmd : {} : START : params: {}'.format( GI.Cmd.name[ self.cmd ], params ) )

					# can
					if self.gui.workflow.can( self.cmd ):

						self.gui.workflow.pre( self.cmd ) # IMPORTANT !!!!!

						# command
						if self.gui.workflow.type( self.cmd ) == GI.Cmd.Type.Command:
							cmd_,self.params_doing=self.gui.workflow.cmd( self.cmd, params ) # p: params dict updated (auto commands) in workflow.cmd()
							if self.gui.workflow.params_check( cmd_ ):
								self.commands.send( cmd_ )
							else:
								self.gui.workflow.post( False ) # TODO: Give somehow a feedback to the user
						# operation
						elif self.gui.workflow.type( self.cmd ) == GI.Cmd.Type.Operation:
							if self.cmd == GI.Cmd.Operation.Pause:
								self.gui.workflow.post( True )
							else:
								self.gui.workflow.on_task( self.cmd, self.gui.workflow.operation_func( self.cmd ) ) # no params
					# can't
					else: # TODO: break sequence or continue ????
						self.gui.workflow.post( False )

					self.gui.after( GI.Value.PollingInterval, self.end )
			else:
				pass
		except:
			helper.handle_exception()

	def end(self):

		try:
			#logging.debug('--> end : current_cmd:{} len( self.cmds ):{}'.format( GI.Cmd.name[ self.workflow.current_cmd ], len( self.cmds ) ))
			
			# cmd finished
			if self.gui.workflow.current_cmd == GI.Cmd.NoCmd: 
				logging.debug('cmds queue : cmd FINISHED')
				#self.print_time( 'cmd' )
				#logging.debug( CmdsQueue.elapsed_time( self.start_time_cmd ) )
				if len( self.cmds ):
					#self.exe()
					self.gui.after( GI.Value.PollingInterval_Short, self.exe )
				else:
					logging.info('NO more cmds to treat');	
					#self.print_time( 'test' )
					self.context.clear()
			# cmd NOT finished
			else: 
				#logging.info('cmd NOT finished');
				if self.gui.control:
					self.gui.control.update_elepsed_time()

				self.gui.after( GI.Value.PollingInterval, self.end )
		except:
			helper.handle_exception()

	@staticmethod
	def format_time( time_ ):
		try:
			minutes, seconds = divmod( time_, 60 )
			#return ' <- cmd : end : time: ({}s.) ({:0>2}:{:0>2})'.format( int(elapsed_time), int(minutes), int(seconds) )
			return '{:0>2}:{:0>2}'.format( int(minutes), int(seconds) )
		except:
			helper.handle_exception()

	@staticmethod
	def elapsed_time(start):
		try:
			return CmdsQueue.format_time( time() - start )
		except:
			helper.handle_exception()

	@staticmethod
	def cmds_s( cmds ):
		try:
			cmds_s_ = '[ '
			for cmd in cmds:
				cmds_s_ += (GI.Cmd.name[ cmd ] + ', ')
			cmds_s_ = cmds_s_[:-2]
			cmds_s_ += ' ]'
			return cmds_s_
			#logging.debug('{}'.format( cmd_s ))
		except:
			helper.handle_exception()
