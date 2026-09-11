
'''
import geomec
def launch_tk_inter():

	#print( sys.argv[ 0 ] )

	root = Tk()

	w = Label(root, text="Hello, world!")
	w.pack()

	root.mainloop()

def main():
	pass

'''

'''
import time # for the thread purpose
from threading import Thread
import datetime

import os
if not os.environ.has_key("PP"):
	import geomec


def _print( val ):

	if not os.environ.has_key("PP"):
		GeomecInterface.do_print( "Py  : " + val )
		print( "Py  : " + val )
	else:
		print( "Py  : " + val )

#---------------------------------------- queue
from Queue import Queue
requests_queue	= Queue()
results_queue	= Queue()

def do_work( cmd ):
	_print( 'Req -> C++_Req_Q' )
	#GeomecInterface.set_cmd( cmd )
	gi = GeomecInterface() 
	gi.set_cmd( cmd )

	#GeomecInterface.do_print( 'ENGLISH' )

def do_work_results( parent, cmd ):
	parent.label['text']='Res received'
	_print( 'Res : Treating' )

def worker( parent ):

	while True:
		# 1)
		#--------------- enqueue request
		cmd = requests_queue.get()
		_print( 'Req <- Py_Req_Q' )
		do_work( cmd )
		requests_queue.task_done()

		# 2)
		#--------------- result : enqueue -> FIXME
		gi = GeomecInterface() 
		cmd = geomec.get_result()
		_print( 'Res <- C++_Res_Q' )
		_print( 'Res -> Py_Res_Q' )
		results_queue.put( cmd )
		#--------------- result : dequeue
		result = results_queue.get()
		_print( 'Res <- Py_Res_Q' )
		do_work_results( parent, result )
		results_queue.task_done()

class Command_Id:

	Cmd_Get_Reservoir_List		= 1
	Cmd_Get_Displacement_List	= 2
	Cmd_Get_Depletion_List		= 3

	Cmd_Get_Reservoir			= 4
	Cmd_Get_Displacement		= 5
	Cmd_Get_Depletion			= 6

	Cmd_Run_Geomec				= 7
	Cmd_Save_Displacement		= 8

	Cmd_None					= 9


class Dialog:

	def __init__(self, master):

		frame = Frame(master)
		frame.pack()
		
		self.button = Button(	frame, text="QUIT", fg="red", command = frame.quit )
		self.button.pack( side = LEFT )
		
		self.cmd = Button(	frame, text="cmd", command = self.add_request)
		self.cmd.pack( side = LEFT )

		self.cmd = Button(	frame, text="clear", command = self.clear )
		self.cmd.pack( side = LEFT )

		self.label = Label( frame, text="Results are shown here" )
		self.label.pack( side = RIGHT )


	def clear( self ):
		self.label['text']=''

	def add_request( self ):
		_print( 'Req -> Py_Req_Q' )
		cmd = ( Command_Id.Cmd_Get_Displacement,  5 ) #depletion_id
		requests_queue.put( cmd )
#---------------------------------------- queue
'''


'''
class ThreadClass( Thread ):

	def __init__(self, w):
		#super( ThreadClass, self).__init__()
		threading.Thread.__init__(self)
		self.w = w

	def run(self):

		self.i=0

		while True:
			now = datetime.datetime.now()
			print( "%s says Hello World at time: %s" % ( self.getName(), now ) )
			time.sleep( 2 )
			self.i += 1
			if self.i == 5:
				print "%s says QUIT at time: %s" % (self.getName(), now)
				#import sys; sys.exit() 
				
				#self.w.quit()
				#self.w.destroy()

				#self.w.textU.insert(1.0,'EVENTO.......\n','OK')

				break

def test_thread():

	w = None
	t = ThreadClass( w )
	t.daemon = True
	t.start()

	while True:
		time.sleep( 2 )
		pass



def func( _a ):

	print( "Original array:" )

	print( _a )

	_b = 10*np.sin( _a )

	print( "Modified array:" )
	print( _b[ 0 ] )

	print( "Extending Python..." )

	import emb
	#print( emb.numargs() )
	print( emb.get_array( _b[ 0 ] ) )

	print( "ADIOS" )

	return _b[ 0 ]
	#print( _array )


#a = np.arange( 15 ).reshape( 3, 5 )	

#print( a )

#def main( argv ):

'''

'''
def main():

	func( sys.argv[ 1 ] )

	#return $1 

	
	#a = np.arange( 15 ).reshape( 3, 5 )	
	#b = 10*np.sin( a )

	#print( a )
	#print( b )

	#return b
	

def func( _array ):

	print( _array )

'''



'''
def do_something():
	
	logging.debug("do something")
	subsinversion().after(1000, do_something)

import time
import threading
import datetime
class ThreadClass(threading.Thread):

	def __init__(self, w):
		#super( ThreadClass, self).__init__()
		threading.Thread.__init__(self)
		self.w = w

	def run(self):

		self.i=0

		while True:
			now = datetime.datetime.now()
			print "%s says Hello World at time: %s" % (self.getName(), now)
			time.sleep( 2 )
			self.i += 1
			if self.i == 5:
				print "%s says QUIT at time: %s" % (self.getName(), now)
				#import sys; sys.exit() 
				
				#self.w.quit()
				#self.w.destroy()

				self.w.textU.insert(1.0,'EVENTO.......\n','OK')

				break

q = Queue()
num_worker_threads = 1

def worker():
    while True:
        item = q.get()
        do_work( item )
        q.task_done()

for i in range( num_worker_threads ):
     t = Thread( target = worker )
     t.daemon = True
     t.start()


for item in source():
    q.put( item )

q.join()       # block until all tasks are done
'''