import logging
from displacement import Displacement
import helper
import IO_gocad

def send_cmd( cmd ):
	logging.debug('interface: send_cmd(): TO IMPLEMENT: {}'.format( cmd ) )

def get_result():
   #cmd_id=GeomecInterface.Request_Id.Cmd_None
   try: 
      cmd_id=4
    	 #params=(1,2,3,'param4')
      filename = "E:\\Inversion\\Disc\\displacement_dummy.txt"
      params=IO_gocad.getmeasure_temp(filename)
      params2=Displacement.transform(params)
      result=(cmd_id,params2)
      logging.debug('interface: get_result(): TO IMPLEMENT: dummy result:{}'.format( result ) )
      return result
   except:
      helper.handle_exception()

def gui_it():
	logging.debug('interface: gui_it(): TO IMPLEMENT' )
	import inversion_GUI
	inversion_GUI.main()