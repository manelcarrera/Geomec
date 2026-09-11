#!/usr/bin/env python

import sys
#import subprocess
import os
import os.path

import logging
import ntpath

from shutil import copy2, copytree


OUTDIR='test_output'


class Script:

	RUN='test_geomec_rgi'
	SAVE='test_geomec_rgi_save'

	"""
	BASH='/bin/bash'

	#$(SHELL) ./test_geomec_rgi "-" $${DESTINATION}/$${TARGET} RockmechDS04 RockmechDS04.gm4 $${DESTINATION}/test-5.log 3 "$$DIAPATH/lib" $${DESTINATION} $(DIASRC) && touch test-5
	CMD_RUN='{0} ./{1} - {3} {4} {5}.gm4 test_output/TEST_{5}_RUN.log 3 {6} {2} {7} && \
	touch test_output/TEST_{5}_RUN'

	#$(SHELL) ./test_geomec_rgi_save "-" $${DESTINATION}/$${TARGET} Gem33_rgi_ds_0 CBMtest.gm4 $${DESTINATION}/test-12.log 3 "$$DIAPATH/lib" $${DESTINATION} $(DIASRC) && touch test-12
	CMD_SAVE='{0} ./{1} - {3} {4} {5}.gm4 test_output/TEST_{5}_SAVE.log 3 {6} {2} {7} && \
	touch test_output/TEST_{5}_SAVE'
	"""

def res_fname( script, model ):

	name='{}/TEST_{}'.format(OUTDIR,model)

	if script == Script.RUN:
		name+='_RUN'.format( model )

	elif script == Script.SAVE:
		name+='_SAVE'.format( model )

	return name

TESTS=(
	# folder, file
	[ Script.RUN, 'RockmechDS04', 				'RockmechDS04' ],
	#[ Script.RUN, 'Square5Model', 				'Square5Model' ],

	#[ Script.SAVE, 'Gem33_rgi_ds_0', 			'CBMtest' ],
	#[ Script.SAVE, 'RockmechDS01_rgi_ds_0', 	'flowSRMS_coarse_D1' ],
	#[ Script.SAVE, 'Gem33_rgi', 				'CBMtest_D0' ],
	#[ Script.SAVE, 'Gem07', 					'Gem07' ],
	#[ Script.SAVE, 'Gem07_mrs_1_restart_1_rgi', 'Gem07' ],
	#[ Script.SAVE, 'Gem44_rgi', 				'SingleHorizon' ],
	#[ Script.SAVE, 'RockmechDS27_rgi', 			'RockmechDS27_export' ],
	#[ Script.SAVE, 'Gem28_rgi', 				'Geomodel4' ],
)

def create_ini_file():
	'''creates geomec.ini from geomec.ini.template setting current value of DIALIB and DIASRC'''

	try:
		DIALIB = os.getenv('DIALIB')
		DIASRC = os.getenv('DIASRC')

		data = open('geomec.ini').read()

		data = data.replace('diana',DIALIB)
		data = data.replace('share',DIASRC)

		o = open('D0.copy/geomec.ini','w')
		o.write( data  )
		o.close()
	except Exception as exception:
		show_error_details()


def show_error_details():
	'''shows exception extended info'''
	exc_type, exc_value, exc_traceback = sys.exc_info()
	traceback_details = {
		'filename': ntpath.basename( exc_traceback.tb_frame.f_code.co_filename ),
		'lineno'  : exc_traceback.tb_lineno,
		'function': exc_traceback.tb_frame.f_code.co_name,
		'type'    : exc_type.__name__,
		'message' : exc_value.message, # or see traceback._some_str() 
	}
	logging.error('Exception:{}'.format( traceback_details ) )

def cmd_( params ):

	try:
		script=params[ 0 ]
		folder=params[ 1 ]
		model=params[ 2 ]

		cmd=''

		if script == Script.RUN:

			current_dir = os.path.dirname( os.path.abspath( __file__ ) )

			# ROOT/test/geomec_rgi/D0.copy

			# this needs to be improved to cover all the cases
			LD_LIBRARY_PATH_=os.environ['LD_LIBRARY_PATH'].replace('../..','../../..')
			# 'LD_LIBRARY_PATH' set works and its needed

			# 'LD_PRELOAD' : adding this it still fails 

			cmd = 'LD_PRELOAD={} '\
			'LD_LIBRARY_PATH={} '\
			'../../../bin/{}/geomec_rgi '\
			'/rockmechfile:{} '\
			'/tempdir:{} >& '\
			'../TEST_{}.log'.format( '/usr/lib64/libGL.so', LD_LIBRARY_PATH_, os.environ['CONF'], folder, current_dir, model )


		elif script == Script.SAVE:

			cmd = Script.CMD_SAVE.format( Script.BASH, script, mode_, bin_, folder, model, dialib_, diasrc_ ) # 0..7

		return cmd

	except Exception as exception:
		show_error_details()


def main( argv ):

	try:
		if not os.environ['CONF']:
			print('\'CONF\' ENVVAR not defined')
			sys.exit(1) # something is wrong

		bin_='../../bin/{}/geomec_rgi'.format(os.environ['CONF'])
		if not os.path.exists( bin_ ):
			print('\'{}\': not found'.format(bin_))
			sys.exit(1) # something is wrong

		if not os.path.exists( 'D0.copy' ):
			copytree('../../Tests/Models/RGI/D0', 'D0.copy')

		if not os.path.exists( OUTDIR ):
			os.makedirs( OUTDIR )

		create_ini_file() # 'geomec.ini.template'  -> 'geomec.ini' ('DIALIB' and 'DIASRC')

		os.chdir('D0.copy')

		for params in TESTS:

			script=params[ 0 ]
			folder=params[ 1 ]
			model=params[ 2 ]

			res_fname_ = res_fname( script, model )
			if os.path.isfile( res_fname_ ):
				print('PASSED ... {}'.format( model ) )
				continue

			cmd=cmd_( params )


			print('test_rgi.py : cmd:{0}'.format( cmd ) )

			# doesn't work
			#args = cmd.split()
			#popen = subprocess.Popen(args, stdout=subprocess.PIPE)
			#popen.wait()
			#res = popen.stdout.read()

			res=os.system(cmd) # returns the exit status


			if res:
				print('test_rgi.py : ERROR : script:\'{0}\' model:\'{1}\' cmd:\'{2}\''.format( script, model, cmd ) )
				break;

			print( 'res:{0}'.format( res ) )

		os.chdir('../')


	except Exception as exception:
		#print( exception )
		show_error_details()


if __name__=='__main__':
	try:
		main( sys.argv[1:] )
	except Exception as exception:
		print( exception )
