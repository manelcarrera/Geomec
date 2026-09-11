import logging
import helper

def send_cmd( cmd ):
	logging.debug('interface: send_cmd(): TO IMPLEMENT: {}'.format( cmd ) )

def get_result():
	try: 
		cmd_id=0
		params=(0,0)
		result=(cmd_id,params)
		logging.debug('interface: get_result(): TO IMPLEMENT: dummy result:{}'.format( result ) )
		return result
	except:
		helper.handle_exception()

def gui_it():
	logging.debug('interface: gui(): TO IMPLEMENT' )
	import gui
	gui.main()