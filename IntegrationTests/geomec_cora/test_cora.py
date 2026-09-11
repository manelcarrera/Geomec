#!/usr/bin/env python

import sys
#import subprocess
import os
import os.path

import logging
import ntpath

from shutil import copy2


""" Comments
1) GET2:
Look at :
1. 'geomec_cora.pro' in branch 5.6.1
2. Implementattion at tests
"""

#from shutil import copyfile

# DESTINATION: 	release / debug
# TARGET: 		geomec_cora

BASEPATH='../../TestData/CORA'
PATH1=BASEPATH
PATH2=BASEPATH+'/WellPathTests'
PATH3=BASEPATH+'/XMLTest'
PATH4=BASEPATH+'/SingleTest'
PATH5=BASEPATH+'/StagedTest'

OUTDIR='test_output'

class Script:
	'''Just an enum with the script types''' 

	GET='GET'
	RUN='RUN'
	VERIFY='VERIFYs'
	GET2='GET2'

TESTS=(

	[ Script.GET, PATH1, 'quadlinear' ],
	[ Script.RUN, PATH1, 'quadlinear' ],

	[ Script.GET, PATH1, 'SimpleModel' ],
	[ Script.RUN, PATH1, 'SimpleModel' ],

	# works fine
	[ Script.VERIFY, PATH2, 'H.WP.0.NWP.0', 0 ],
	[ Script.VERIFY, PATH2, 'H.WP.0.NWP.1', 1 ],
	[ Script.VERIFY, PATH2, 'H.WP.0.NWP.4', 4 ],
	[ Script.VERIFY, PATH2, 'H.WP.1.NWP.0', 1 ],
	[ Script.VERIFY, PATH2, 'H.WP.1.NWP.1', 2 ],
	[ Script.VERIFY, PATH2, 'H.WP.4.NWP.0', 4 ],
	[ Script.VERIFY, PATH2, 'H.WP.4.NWP.4', 8 ],
	[ Script.VERIFY, PATH2, 'T.WP.0.NWP.0', 0 ],
	[ Script.VERIFY, PATH2, 'T.WP.0.NWP.1', 1 ],
	[ Script.VERIFY, PATH2, 'T.WP.0.NWP.4', 4 ],
	[ Script.VERIFY, PATH2, 'T.WP.1.NWP.0', 1 ],
	[ Script.VERIFY, PATH2, 'T.WP.1.NWP.1', 2 ],
	[ Script.VERIFY, PATH2, 'T.WP.4.NWP.0', 4 ],
	[ Script.VERIFY, PATH2, 'T.WP.4.NWP.4', 8 ],

	# FIXME: temporary file names not Ok, all tests uses same names so results overwrite previous results
	[ Script.GET2, 	PATH3, 'Hexahedron', '2.0', 'XML' ],
	[ Script.GET2, 	PATH3, 'Hexahedron', '1.7', 'TXT' ],
	[ Script.GET2, 	PATH3, 'Hexahedron', '1.7', 'NO-version' ],

	# works fine
	[ Script.GET, 	PATH4, 'SingleTest' ],
	[ Script.GET, 	PATH5, 'StagedTest' ],
)

# Functions

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


def res_fname( script, model, type_ ):
	'''result file name. Useful to know if the test has been already passed succesfully'''

	name='{}/TEST_{}'.format( OUTDIR, model )

	if script in [ 	Script.GET,Script.RUN,
					Script.VERIFY]:
		name+='_{}'.format( script )
	elif script == Script.GET2:
		name+='_{}'.format(type_)

	name+='_OK'

	return name


def create_ini_file():
	'''creates geomec.ini from geomec.ini.template setting current value of DIALIB and DIASRC'''

	try:
		DIALIB = os.getenv('DIALIB')
		DIASRC = os.getenv('DIASRC')

		#if not DIALIB or not DIASRC: 
		print('DIALIB:{} DIASRC:{}'.format(DIALIB,DIASRC))

		data = open('geomec.ini.template').read()

		data = data.replace('diana',DIALIB)
		data = data.replace('share',DIASRC)

		o = open('geomec.ini','w')
		o.write( data  )
		o.close()
	except Exception as exception:
		show_error_details()


def cmd_( params ):
	'''builds the complet command from test parameters: name and path of model, if its get/run/verify, etc.'''

	script=params[ 0 ]
	path=params[ 1 ]
	model=params[ 2 ]

	cmd=''

	type_=''
	version=''
	if script == Script.GET2:
		type_=params[4]
		version=params[3]

	res_fname_ = res_fname( script, model, type_ )

	if os.path.isfile( res_fname_ ):
		print('PASSED ... model:{}'.format( model ) )
		return cmd

	if script in ( 	Script.GET, 
					Script.VERIFY, 
					Script.GET2):
		
		# only 'GET2' needs 'version' 
		cmd = \
		'../../bin/{4}/geomec_cora ' \
		'{0}/{1}.gm4 ' \
		'getmodelinfo '\
		'{3} '\
		'{2}/{1}.getmodelinfo '\
		'{2}/{1}.summary_result_file '\
		'> {2}/{1}.get.log 2>&1'.format( 
			path, 		# 0
			model, 		# 1
			OUTDIR, 	# 2
			version, 	# 3
			os.environ['CONF'] ) 	# 4

	elif script == Script.RUN:

		version='1.3'

		cmd = \
		'../../bin/{4}/geomec_cora '\
		'{0}/{1}.gm4 '\
		'runmodel '\
		'{3} '\
		'{0}/{1}.parameterfile '\
		'{0}/{1}.selectedlsfs '\
		'{2}/{1}.responseparameterfile '\
		'{2}/{1}.summary_result_file '\
		'minimum_output '\
		'> {2}/{1}.get.log 2>&1'.format( 
			path, 		# 0	
			model, 		# 1
			OUTDIR, 	# 3	
			version,	# 4	 
			os.environ['CONF'] ) # 5	 

	return cmd


def main( argv ):
	try:

		if not os.path.exists( OUTDIR ):
			os.makedirs( OUTDIR )

		if not os.environ['CONF']:
			print('\'CONF\' ENVVAR not defined')
			sys.exit(1) # something is wrong

		bin_='../../bin/{}/geomec_cora'.format(os.environ['CONF'])
		if not os.path.exists( bin_ ):
			print('\'{}\': not found'.format(bin_))
			sys.exit(1) # something is wrong

		create_ini_file() # 'geomec.ini.template'  -> 'geomec.ini' ('DIALIB' and 'DIASRC')

		for params in TESTS:
	
			cmd=cmd_( params ) # builds the command
			print('cmd: {}'.format( cmd ) )

			# cmd empty when test already passed
			if not cmd:
				continue

			model=params[ 2 ]
			script=params[ 0 ]

			# only cmd for 'GET' and 'RUN' and first cmd for 'GET2' and 'VERIFY'
			res=os.system(cmd) # returns the exit status
			if res: # something wrong
				print('test_cora.py : ERROR : script:\'{}\' model:\'{}\' cmd:\'{}\''.format( script, model, cmd ) )
				#FIXME
				#break 
			else: # success but some tests needs more commands ('GET2' and 'VERIFY') to be performed
				type_=''
				# --------------------------------------------- GET2
				# special case: sequence of commands
				if script == Script.GET2:
					type_=params[4]
					path=params[ 1 ]

					cmd = 	'dos2unix -n '\
							'{0}/{1}.reference.xml '\
							'{2}/{1}.reference.xml'.format( 
								path, 
								model, 
								OUTDIR )
					res=os.system(cmd)
					if not res: #success
						cmd='chmod -f +w {}/{}.reference.xml'.format(OUTDIR,model)
						res=os.system(cmd)
						if not res:
							# FIXME: doesnt exist 'model.getmodelinfo.xml'
							cmd='cmp '\
								'{0}/{1}.getmodelinfo.xml '\
								'{0}/{1}.reference.xml'.format(
									OUTDIR,
									model)
							res=os.system(cmd)
							if not res:
								cmd='rm -f {}/{}.reference.xml'.format(OUTDIR,model)
								res=os.system(cmd)
				# --------------------------------------------- VERIFY
				# special case: sequence of commands
				elif script == Script.VERIFY:
					lines=params[3]
					file_='{}/{}.getmodelinfo'.format(OUTDIR,model)

					count=0
					with open(file_) as f:
					    for line in f:
					    	if line.startswith('well:New Wellpath'):
					    		count+=1
					#print('model:{} wells: count:{} lines:{}'.format(model,count,lines))
					res = False if count == lines else True
				# --------------------------------------------- GET, RUN
				# regular commands ('GET' and 'RUN')
				else:
					res = False
				# ---------------------------------------------

				if not res: #success
					res_fname_ = res_fname( script, model, type_ )
					os.system('touch {}'.format(res_fname_))


				print( 'res:{} ... model:{}'.format( not res, model ) )


	except Exception as exception:
		#print( exception )
		show_error_details()


if __name__=='__main__':
	try:
		main( sys.argv[1:] )
	except Exception as exception:
		print( exception )


# (1)
#args = ('/bin/bash test_getmodelinfo', mode_, bin_, 'quadlinear.gm4' )
#args = ("bin/bar", "-c", "somefile.xml", "-d", "text.txt", "-r", "aString", "-f", "anotherString")
#Or just:
#args = "bin/bar -c somefile.xml -d text.txt -r aString -f anotherString".split()
#popen = subprocess.Popen(args, stdout=subprocess.PIPE)
#popen.wait()
#output = popen.stdout.read()
