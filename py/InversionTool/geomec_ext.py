import logging
import importlib
import helper #handle_exception

'''
def set_num():
	pass

def set_array():
	pass

def get_num():
	pass

def get_array():
	pass

def get_cmd():
	pass
'''
def reservoir( filename ):
	logging.debug('interface_ext: reservoir(): Run GM or implement the interface : filename:{}'.format( filename ) )
	import IO_gocad
	return IO_gocad.readdata( filename )

def displacement( filename ):
	logging.debug('interface_ext: displacement(): Run GM or implement the interface: filename:{}'.format( filename ) )
	import IO_gocad
	return IO_gocad.getmeasure( filename )

def func( params ):

	try:
		logging.debug('interface_ext: func(): Run GM or implement the interface: params:{}'.format( params ) )

		module_name,func_name,param=params

		my_module = importlib.import_module( module_name )

		my_method = getattr( my_module, func_name )
		result = my_method( param )
		return result
	except:
		helper.handle_exception()

	'''
	if func == 'readdata':
		return reservoir( param )
	elif func == 'getmeasure':
		return displacement( param )
	'''
