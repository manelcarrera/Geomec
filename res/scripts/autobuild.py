#!/usr/bin/env python

#
# last update: mcr 2020-08-20
#

from shutil import copy2
import os
import sys
import logging
import ntpath
import datetime
import os.path
from glob import glob
from datetime import datetime #today, to know the year
from datetime import timedelta
#import subprocess

#######################################################################
#
#
#	global functions and CError (the latest not used so far)
#
#
#######################################################################

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


class CError:
	'''error codes
	not used by now'''
	configuration=-1
	profile=-2
	tf=-3
	ws_base=-4
	drop_base=-5
	localdir=-6
	dropdir=-6
	chdir=-7

#######################################################################
#
#
#				DEF -> Contants
#
#
#######################################################################

class DEF:
	conf='release'

	prefix='gm'

	if sys.platform == 'win32':
		root='c:/'
	else:
		root='/'
	scratch=root+'scratch'

	#
	soft='/glb/data/geomechanics/software'

	#TFS
	tf_collection='https://sede-pt-ssw.visualstudio.com/DefaultCollection'
	token='{}/tokens/sede_vso.token'.format(soft)

	drops='{}/DROPS'.format(soft)

	# qmake and make
	qmake_target='geomec.pro'
	#make_target='installer'
	make_target='installer-no-test' # !!!!!!!!

	# logs
	logfile='build.log'
	
	TEST_RGI='./IntegrationTests/geomec_rgi'
	exe_files=[
		'qmake/check.environment.sh',
		'qmake/create.diana.pri',
		#
		'lib/GeomecStringTable/testYear',
		# TEST_RGI 
		'{}/create_installation'.format(TEST_RGI),
		'{}/install'.format(TEST_RGI),
		'{}/run_geomec_rgi'.format(TEST_RGI),
		'{}/installer_run_geomec_rgi'.format(TEST_RGI),
		'{}/installer_test_geomec_rgi'.format(TEST_RGI),
		#
		'Tests/CreateRGInterfaceTests/test_geomec_rgi'
	]

#######################################################################
#
#
#						Util
#
#
#######################################################################

class Util:

	@staticmethod
	def exe(cmd,res=False):
		'''only commands go to the std output
		run the command goes to teh logs file'''

		print('cmd:{}'.format(cmd))
	
		cmd2=cmd+' >> {} 2>&1'.format(DEF.logfile)

		#cmd2 = 'echo {} >> {} 2>&1'.format(cmd,DEF.logfile)
		#os.system(cmd2)

		if res:
			return os.popen(cmd).read() # using cm2 doesnt work
			#p = subprocess.Popen(["cat", DEF.token], stdout=subprocess.PIPE)
			#return p.communicate()[0].strip()
		else:
			os.system(cmd2)

	@staticmethod
	def to_file(msg):
		Util.exe('echo {}'.format(msg))

	@staticmethod
	def timestamp_to_file():
		#
		# it writes one line with the timestamp to the logs file
		#
		now=datetime.datetime.now()
		date_s = now.strftime('%Y%m%d_%H%M%S')

		cmd='echo'
		Util.exe(cmd)

		cmd='echo ++ {} ++'.format(date_s)
		Util.exe(cmd)

		cmd='echo'
		Util.exe(cmd)


#######################################################################
#
#
#						Params
#
#
#######################################################################

class Params:
	#
	# -> about btype:	-> appears in 'VERSION' and in the binary metadata
	#
	# Look at 'version.h':
	#
	# GM_BUILD_TYPE_CI					-> "CI "
	# GM_BUILD_TYPE_TEST				-> "test "
	# GM_BUILD_TYPE_NIGHTLY				-> "dev nightly "
	# GM_BUILD_TYPE_RELEASE_NIGHTLY		-> "nightly "
	# GM_BUILD_TYPE_RELEASE				-> ""
	#
	#	if empty						-> "dev "
	#
	# btype='NIGHTLY'    # 'NIGHTLY' p.e
	#

	#
	# mcr 2020-10-08:
	#
	# NOT CLEAR WHAT THE STRING MUST BE FOR EACH CASE !!! 
	# I assume CI, TEST, NIGHTLY, RELEASE and ''
	# but what about RELEASE NIGHLTY ?
	#

	#
	# define here static members
	#

	#
	# members:
	# - conf:			releease / debug					-> only for the files location -> bin/'release' f.e
	# - type:			RELEASE								-> BUILD_TYPE={} make,qmake
	# - source:			Trunk / 'branches/Geomec 5.7.1'
	# - version:		????
	#
	def __init__(self,argv):
		#
		# define here elements of the object
		#
		self.__default()
		#
		self.__parse(argv)
		#
		self.__str__() # shouldn't it be 'self.print()' instead?

	def __default(self):
		#
		# edit this method if no arguments passed to the script ('autobuild .py')
		#

		#
		# os.environ['CONF'] -> raises exception if 'CONF' doesn't exists
		#

		self.conf='release'
		#
		# make in debug mode it works fine but for the UserDLLs folder ... 'so' are always in UserDLLs/ but apps .pro looks at UserDLLs/debug
		# 
		#
		#self.conf='debug'
		#
		# don't do this by now
		#
		#if os.environ.get('CONF'):
		# self.conf= os.environ['CONF']
		#
		#
		#
		self.btype='RELEASE'
		#
		#
		#
		self.source='Trunk'
		#self.source='branches/Geomec 5.7.1'
		#
		#
		#
		self.version=''
		#	
		#
		#
		self.use=''

	@staticmethod
	def usage_2020_10():
		'''typical usage method to know how to use the scripts'''
		print('usage: python autobuild.py [source] ... (default: \'Trunk\')')
		print('usage: python autobuild.py [use] ... (Example: \'gm_20201006_083932\')')
		print('default tfs source: \'Trunk\'')
		print('')
		print('temporarily disabled:')
		print('usage: python autobuild.py [conf][btype][source][version]')

	@staticmethod
	def usage_2020_11():
		'''typical usage method to know how to use the scripts'''

		print('')
		print('--usage--')
		print(' -python autobuild.py source=\'branches/Geomec 5.8.0\'')
		print(' -python autobuild.py wks=gm_20200101_090000 ... all but get')
		print(' -python autobuild.py get=\'branches/Geomec 5.8.0\' ... get-only')
		print(' -python autobuild.py make=gm_20200101_090000 ... make-only')
		print(' -python autobuild.py inst=gm_20200101_090000 ... create_installation-only')
		print(' -python autobuild.py drop=gm_20200101_090000 ... drop-only')
		print(' -python autobuild.py source_req=gm_20200101_090000')
		print(' -python autobuild.py version=gm_20200101_090000')
		print(' -python autobuild.py list=2020/12 ... also list=2020')
		print(' -python autobuild.py versions=2020/12 ...  ... also list=2020')
		print(' -python autobuild.py test=gm_20200101_090000 1,2')
		print(' -python autobuild.py tests_list=gm_20200101_090000')
		print(' -python autobuild.py fix-transmissions')
		print(' -python autobuild.py third-party-versions')
		print(' -python autobuild.py module')
		print(' -python autobuild.py modified=gm_20200101_090000')
		print(' -python autobuild.py map=gm_20200101_090000')
		print(' -python autobuild.py envvar=PATH')
		print('')
		print('--notes')
		print(' -no wks name provided -> last one assumed')
		print(' -no year/month provided -> current ones assumed')
		print('')

	@staticmethod
	def usage():
		Params.usage_2020_11()

	def __parse_2020_10(self,argv):
		#
		# by now none or one param allowed
		#
		argc=len(argv)
		#
		# if 1 param provided
		#
		if argc > 0:
			arg=argv[0]
			#
			# 1) usage
			#
			if arg=='-h' or arg=='--help':
				Params.usage_2020_10()
				sys.exit(0)
			#
			# 2) reuse wks
			#
			elif DEF.prefix in arg:
				y,m = Workspace.year_month(arg)
				path_='{}/{}/{}/{}'.format(DEF.scratch,y,m,arg)
				if os.path.isdir( path_ ): 
					self.use=arg
				else:
					print('ERROR: wks \'{}\' doesn\'t exist\n'.format(arg))
					Params.usage_2020_10()
					sys.exit(0)
			#
			# 3) provided tfs soruce other than default (Trunk) 
			#
			else:
				self.source=argv[0]

	#
	# members: k, v, use and source
	#
	# --use cases--
	#
	# -h
	# --help
	#
	# wks=gm_20201127_074526	# params.use
	# source=Trunk				
	#
	# make=gm_20201127_074526
	# inst=gm_20201127_074526
	# drop=gm_20201127_074526
	# get=Trunk					# params.source
	#
	def __parse_2020_11(self,argv):
		try:
			# by now none or one param allowed
			#
			self.k='default'
			#
			sep='='
			#
			argc=len(argv)
			#
			# if 1 param provided
			#
			if argc > 0:
				arg=argv[0]
				#
				# 1) usage
				#
				if arg=='-h' or arg=='--help':
					self.k='help'
				#
				# 2) arg Ok
				#
				elif sep in arg:

					a=arg.split(sep)
					self.k,self.v=[a[i] for i in (0,1)] 

					if self.k=='wks':
						self.use=self.v

					if self.k=='test':
						self.v2=argv[1]

					elif self.k in ['source','source_req','get']:
						self.source=self.v
				#
				# 3) arg not Ok: take the deafult one: 2020 / last wks
				#
				else:
					self.k=arg
					today = datetime.today()
					#
					# if no YEAR/MONTH filter provided, current one is asseumed
					#
					filter='{}/{}'.format(today.year,today.month)
					#
					# 3a) no params
					#
					if self.k in ['fix-transmissions','last','third-party-versions','module']: # no need params
						pass
					#
					# 3b) params by default
					#
					elif self.k in ['list','versions']: # need year or year/month
						self.v=filter
						print('{}={}'.format(self.k,self.v))
					#
					#
					#
					elif self.k in ['make','inst','drop','wks','source_req','version','test','tests_list','res','delete','modified','map']: # need wks name
						self.v=Tools.last()
						print('{}={}'.format(self.k,self.v))
					#
					#
					#
					else:
						self.k='help'
			#
			# no-params
			#
			else:
				pass # default case: get Trunk, make, create_installation and drop
				
		except Exception as exception:
			show_error_details()

	def __parse_before_2020_10(self,argv):
		#
		# 2020-10-08, mcr: 
		#	now a days it should accept only one paarmeter: source (trunk or branch)	
		#	to have all the rest, scrips needs to be updated
		#

		argc=len(argv)

		#
		# at this moment not really usefull, 'autobuild.py' and 'geomec.pro' should be adadpted to take this parameter in account
		# by now it needs to be HC to 'release'
		#
		if argc > 0:
			self.conf=argv[0]
		#
		# all Ok but the needed string for the RELEASE NIGHTLY
		#
		if argc > 1:
			self.btype=argv[1]
		#
		# at 2020-10-08 these two are beig used:
		#	'Trunk'
		#	'branches/Geomec 5.7.1'
		#
		if argc > 2:
			self.source=argv[2]
		#
		# not in use
		#
		if argc > 3:
			self.version=argv[3]

	def __parse(self,argv):
		self.__parse_2020_11(argv)

	def __str__(self):
		return 'build config: conf:\'{}\' btype:\'{}\' source:\'{}\' version:\'{}\' use:\'{}\''.format(
			self.conf,self.btype,self.source,self.version,
			self.use)

#######################################################################
#
#
#						Workspace
#
#
#######################################################################

class Workspace:
	#
	# members:
	# - use
	# - name
	# - path
	#
	# from paarms for __tfs it needs: 
	# - source
	# - version
	#
	def __init__(self, params):
		#
		# set 'use' var in __init__ to re-use a wks (instead of creating a new one)
		#
		#self.use=''
		self.use=params.use
		#self.use='gm_20201006_083932'
		#
		self.name=''
		self.path=''
		#
		self.params=params

	@staticmethod
	def year_month(build):
		'''returns year and month given wks folder name / also valid for a this format date_time yyyyMMdd_hhmmss'''
		#
		# 1) Example: 'gm_20201006_083932' -> 2020,10
		# 
		# 2) Example: '20201006_083932' -> 2020,10 (ALSO POSSIBLE)
		#
		#if 'gm_' in build:
		if DEF.prefix in build: #'gm_'
			date=build[3:] # remove gm_
		else:
			date=build

		year = date[:4]	# chars from start to the 4th
		month = date[4:6] # chars from 4th to 6th 
		return (year,month)
		
	#
	# gm_DATE_TIME
	#
	def __create_dir(self):
		'''creates the wks dir'''

		now=datetime.datetime.now()
		date_s = now.strftime('%Y%m%d_%H%M%S')

		y,m=Workspace.year_month(date_s)

		self.id=date_s
		#self.name ='gm_{}'.format(date_s)
		self.name ='{}_{}'.format(DEF.prefix,self.id)
		self.path='{}/{}/{}/{}'.format(DEF.scratch,y,m,self.name)
		os.makedirs(self.path)
		print('wks.path:{}'.format(self.path))

	#
	# gm_DATE_TIME_build
	#
	# __build__/gm_DATE_TIME
	#
	def __create_dir_build(self):
		#self.build ='{}_build'.format(self.path)
		self.build =Tools.build(self.name)
		print('wks::create_dir_build : {}'.format(self.build))
		#
		os.makedirs(self.build)
		print('wks.build:{}'.format(self.build))

	def __set_props(self,path):
		print('wks::set_props : {}'.format(path))
		current_dir=os.getcwd()
		#
		os.chdir(path)
		#
		Util.exe('chgrp g_epgm00 .')
		Util.exe('chmod g+rws .')
		Util.exe('chmod -R +w .')
		#
		os.chdir(current_dir)

	def __tfs(self):
		'''connects to TFS and gets sources'''
		#
		# it needs: 
		#	from params: source + version
		#	regular members: name + path

		print('wks::tfs')
		#
		# 1) new wks
		#
		cmd='cat {}'.format(DEF.token)
		token_=Util.exe(cmd,True)
		tf_login='any_name,{}'.format(token_)

		cmd='tf workspace -new {} -collection:{} -login:{} -location:local'.format(self.name,DEF.tf_collection,tf_login)
		Util.exe(cmd)
		#
		# 2) link to TFS
		#
		cmd='tf workfold -map -workspace:{} -collection:{} "$/Geomec/{}" {}'.format(self.name,DEF.tf_collection,self.params.source,self.path)
		Util.exe(cmd)
		#
		# 3) get files (version also possible)
		#
		# version has never worked to me (mcr 2020-10-06)
		#
		version_s=''
		if self.params.version:
			version_s='-version={}'.format(self.params.version)

		cmd='tf get -recursive -force {} .'.format(version_s)
		Util.exe(cmd)

	def create(self):
		'''creaates the wks and gets the sources'''

		#
		# - source: TFS sources: trunk or a branch
		# - btype (CI,RELEASE,etc.): needs to be verified 
		# - release/debug: taken from the environment
		#
		# params example:
		#
		# python autobuild.py debug CI Trunk 320614 
		#
		# (conf,type,source,version)
		#

		#
		# - source: '$Geomec/Trunk' for example
		# - version: to be verified, seems it doesn't work
		#
		try:
			self.__create_dir()
			self.__create_dir_build()
			#
			#
			#	
			os.chdir(self.path)
			print('wks::create : current dir:\'{}\''.format(os.getcwd()))

			#Util.timestamp_to_file()
			#
			Util.to_file('source: {}'.format(self.params.source))
			Util.to_file('wks: {}'.format(self.name))
			#
			#
			#
			Util.exe('chgrp g_epgm00 .')
			Util.exe('chgrp g_epgm00 ..')		# month, just in case
			Util.exe('chgrp g_epgm00 ../..')	# year, just in case
			#
			# FIXME
			#
			Util.exe('chmod g+rws .') #this doesn't work, files permissions: '- r-- r-- ---'
			#
			#
			#
			self.__tfs()
			#
			#
			#
			Util.exe('chmod -R +w .') # this needs to be done as the command before doesn't work ok ('chmod g+rws .')
			#
			# make some files executable
			#
			try:
				for file in DEF.exe_files:
					cmd='chmod +x {}'.format(file)
					Util.exe(cmd)
			except Exception as exception:
				show_error_details()
			#
			#
			#
			self.__set_props(self.build)	# group, 's', +w
			#
			#
			#

		except Exception as exception:
			show_error_details()

#######################################################################
#
#
#						Build
#
#
#######################################################################

class Build:
	#
	# members:
	# - params
	# - wks
	#
	def __init__(self,params):
		self.params=params

		#
		# FIXME: needed patch, to be solved
		#
		DEF.conf=self.params.conf

		self.wks=Workspace(self.params)

	def __make(self):
		#
		# it needs:
		# - type
		# - qmake file (.pro)
		# - qmake target: what to execute in .pro file
		#

		#
		# mcr 2020-10-08:
		#
		# make target hard-coded to 'installer-no-test.target' (DEF.make_target)
		# it does:
		#	+ make release
		#	+ cd IntegrationTests/geomec_rgi && python create_installation.py && cd -
		#
		# to build in debug mode 'geomec.pro' must be updated
		#

		#
		# qmake
		#
		cmd='BUILD_TYPE={} qmake {}'.format(self.params.btype,DEF.qmake_target)
		Util.exe(cmd)

		#
		# make
		#
		cmd='BUILD_TYPE={} make {}'.format(self.params.btype,DEF.make_target)
		Util.exe(cmd) # dont execute this with popen, otherwise make logs dont go to the log file

	def __drop(self):
		#
		# it needs:
		# - drop destination: DEF.drops + wks name
		# - files to be droped
		#
		try:	
			#
			# this is done twice in some cases but it doesn't matter
			#
			os.chdir(self.wks.path)
			#print('current dir:\'{}\''.format(os.getcwd()))
			#
			# 
			#
			origin='{}/bin/{}/install_files'.format(self.wks.build, DEF.conf)
			files=[
				'geomec_rgi.tar.bz2',
				'install',
				'README.install',
				'VERSION'
			]
			#
			# 
			#
			#if os.path.isdir( DEF.INSTALL_FILES_PATH ): 
			if os.path.isdir( origin ): 
				# copy files to drop dir
				# 'build.log' also copied
				try:
					y,m=Workspace.year_month(self.wks.name)

					DEST='{}/{}/{}/{}'.format(DEF.drops,y,m,self.wks.name)

					#DEST='{}/{}'.format(DEF.drops,self.wks.name)
					#print('drop: {} -> {}'.format(origin,DEST))
					print('drop to: {}'.format(DEST))

					if not os.path.isdir(DEST): # only needed on testing 
						os.makedirs(DEST)

					#for file in DEF.files:
					for file in files:
						#file_='{}/{}'.format(self.wks.path,file)
						url='{}/{}'.format(origin,file)
						if os.path.isfile(url):
							copy2(url,DEST)
					copy2(DEF.logfile,DEST)

				except Exception as exception:
					show_error_details()
			#------------------------------- c1) fails: remove files
			# if fails delete all but logs file
			# disabled by now
			else:
				print('origin doesnt exist:\'{}\''.format(origin))
				#pass
				# clean wks
				#cmd='tf workspace -delete {}'.format(wks)
				#exe(cmd)
				# remove wks folder
				#rmtree(wks_fullpath)
				# remove files in tmp folder
				#files = glob(os.environ['TMP'])
				#for f in files:
				#    os.remove(f)
		except Exception as exception:
			show_error_details()
	#
	# drop only
	#
	def drop(self,use):

		print('drop-only:{}'.format(use) )

		try:	
			self.wks.use=use
			self.wks.name=self.wks.use
			self.wks.path=Tools.path(self.wks.name)
			self.wks.build=Tools.build(self.wks.name)

			if os.path.isdir( self.wks.path ): 
				self.__drop()
			else:
				print('wks.path:{} : ERROR: path not found'.format(self.wks.path))
		except Exception as exception:
			show_error_details()

	def __res(self):
		'''make result'''
		#
		# 'Tests' binary is the last one to be created -> if it exists we assume make has finished properly
		#
		res=True if os.path.isfile('{}/bin/{}/Tests'.format(self.wks.build,self.params.conf)) else False
		print('res make:{}'.format(res))
		return res

	@staticmethod
	def res(wks,conf='release'):
		'''make result'''
		#
		# 'Tests' binary is the last one to be created -> if it exists we assume make has finished properly
		#
		#y,m=Workspace.year_month(wks)
		#path_='{}/{}/{}/{}'.format(DEF.scratch,y,m,wks)
		#path_=Tools.path(wks)
		#build ='{}_build'.format(path_)
		build =Tools.build(wks)
		file='{}/bin/{}/Tests'.format(build,conf);
		res=True if os.path.isfile(file) else False
		print('file:{}'.format(file))
		print('res make:{}'.format(res))
		return res

	def __save_log_file(self):
		'''save log file to scratch folder'''

		logfile_='{}_{}'.format(self.wks.build,DEF.logfile)
		copy2(DEF.logfile,logfile_)
		Util.exe('chgrp g_epgm00 {}'.format(logfile_))

	def __check_envvars(self):
		'''exits if not Ok'''
		#
		# skip in windows
		#	
		if sys.platform == 'win32':
			return

		if not os.getenv('DIAROOT'):
			print('Execute first \'. ~/software/SCRIPT/setenv\'')
			sys.exit(1)

		if 'eclipse_tfs' not in os.getenv('PATH'):
			print('Execute first \'. ~/software/SCRIPT/setenv TFS\'')
			sys.exit(1)

	def __sanity_checks(self):
		'''to be implemented'''
		#
		# skip in windows
		#	
		if sys.platform == 'win32':
			return

		error_id=0
		if error_id:
			print('sanity checks error:{}'.format(error_id))
			sys.exit(1)

	def run(self):
		#
		# use or create a wks and get sources + make + drop files
		#
		try:
			#print('build::run >>')
			#
			# preliminar checks -> exits if not Ok
			#
			self.__check_envvars()
			self.__sanity_checks()
			#
			# a) reuse code 
			#
			if self.wks.use: 
				#
				#
				#
				self.wks.name=self.wks.use
				#y,m=Workspace.year_month(self.wks.name)
				#self.wks.path='{}/{}/{}/{}'.format(DEF.scratch,y,m,self.wks.name)
				self.wks.path=Tools.path(self.wks.name)
				#self.wks.build ='{}_build'.format(self.wks.path)
				self.wks.build=Tools.build(self.wks.name)
				#
				# if it doesn't exist it raises an exception: so Ok
				#
				os.chdir(self.wks.path)

				#print('wks.path:{}'.format(self.wks.path))
				Util.timestamp_to_file()
			#
			# b) create a new wks + get code
			#
			else:
				self.wks.create()

			print('wks.path:{} {}'.format(self.wks.path, '' if self.wks.use else '[NEW]' ))
			#
			#
			#
			self.__make()
			#
			#
			#
			try:
				Tools.create_installation(self.wks.name)
			except Exception as exception:
				show_error_details()
			#
			#
			#
			self.__save_log_file()
			#
			# move installation files to drops folder
			#
			if self.__res():
				self.__drop()
			#print('build::run <<')
			#
			#
			#
		except Exception as exception:
			show_error_details()



#######################################################################
#
#
#						Tools
#
#
#######################################################################
class Tools:

	class Tests:
		@staticmethod
		def test_01():
			'''year, month'''
			try:
				now=datetime.datetime.now()
				date_s = now.strftime('%Y%m%d_%H%M%S')

				y,m=Workspace.year_month(date_s)

				name='{}_{}'.format(DEF.prefix,date_s)
				path_='{}/{}/{}/{}'.format(DEF.scratch,y,m,path_)

				os.makedirs(path_) # it makes dirs recursively
				print('path:{}'.format(path_))

			except Exception as exception:
				show_error_details()
		@staticmethod
		def test_02():
			'''create dirs'''
			try:
				NO_PARAMS=''
				p=Params(NO_PARAMS)
				w=Workspace(p)
				w._Workspace__create_dir()
				w._Workspace__create_dir_build()
			except Exception as exception:
				show_error_details()

	@staticmethod
	def drop_path(wks):
		'''drop path given a wks name''' 
		y,m=Workspace.year_month(self.wks.name)
		path='{}/{}/{}/{}'.format(DEF.drops,y,m,wks)
		return path
	#
	# drop-only
	#
	@staticmethod
	def drop(wks):
		'''copies installation files from __build__/wks/bin/release/install_files to software/DROPS folder'''
		try:
			NO_PARAMS=''
			p=Params(NO_PARAMS)
			b=Build(p)
			b.drop(wks)
		except Exception as exception:
			show_error_details()

	@staticmethod
	def res(wks,conf='release'):
		'''checks build result'''
		Build.res(wks,conf)

	@staticmethod
	def path(wks):
		'''wks path given a wks name'''
		y,m=Workspace.year_month(wks)
		path_='{}/{}/{}/{}'.format(DEF.scratch,y,m,wks)
		return path_

	@staticmethod
	def build(wks):
		'''wks build path'''
		y,m=Workspace.year_month(wks)
		path_='{}/{}/{}/__build__/{}'.format(DEF.scratch,y,m,wks) # -> NEW
		return path_

	@staticmethod
	def create_installation(wks):
		'''creates installation into SOURCES/wks/IntegrationTests/geomec_rgi/geomec_rgi_TMP'''
		'''then zips it into SOURCES/__build_/wks/bin/release/install_files/geomec_rgi.tar.bz2'''
		try:
			path_=Tools.path(wks)
			path__='{}/IntegrationTests/geomec_rgi'.format(path_)
			#
			import imp
			module = imp.load_source('create_installation', '{}/create_installation.py'.format(path__)) #'module.name'
			#
			current_dir=os.getcwd()
			os.chdir(path__)
			#
			ci=module.CreateInstallation()
			ci.run()
			#
			os.chdir(current_dir)

		except Exception as exception:
			show_error_details()

	@staticmethod
	def get(source):
		'''gets the code sources'''
		try:
			#
			# FIXME: veirfy it
			#
			p=Params('')
			p.source=source
			w=Workspace(p)
			w.create()
		except Exception as exception:
			show_error_details()

	@staticmethod
	def exist(wks):
		'''in: wks name'''
		path_=Tools.path(wks)
		res= True if os.path.isdir( path_ ) else False
		return res

	@staticmethod
	def source(wks):
		'''given a wks name, returns the TFS source, ex.: Trunk, branches/Geoemc 5.8.0'''
		path_=Tools.path(wks)
		build_log='{}/build.log'.format(path_)
		with open(build_log) as f:
			first_line = f.readline()
			key='source:'
			value=''
			if key in first_line:
				value=first_line[-len(key):].strip()
		return value

	@staticmethod
	def source_req(wks):
		'''given a wks name, prints the TFS source, ex.: Trunk, branches/Geoemc 5.8.0'''
		print('source: {}'.format(Tools.source(wks)))

	@staticmethod
	def list(year):
		'''workspaces paths list'''
		cmd='tree -L 2 {}/{} --noreport | grep -v build'.format(DEF.scratch,year)
		os.system(cmd)

	@staticmethod
	def ymn(wks):
		'''year,month,name given a path'''
		items=wks.split('/')
		return [items[2],items[3],items[4]]

	@staticmethod
	def paths(year):
		'''given a year, workspaces paths list'''
		maxdepth=2 if '/' not in str(year) else 1
		cmd='find {}/{} -maxdepth {} -type d | grep gm | grep -v build'.format(DEF.scratch,year,maxdepth)
		paths=os.popen(cmd).read()
		paths_=[]
		for path in paths.splitlines():
			paths_.append(path)
		return paths_

	@staticmethod
	def versions(year):
		'''given a year, prints: wks path: version'''
		paths=Tools.paths(year)
		month_=''
		for path in paths:
			#
			month=Tools.ymn(path)[1] # year,month,name
			if month!=month_:
				print(month)
			month_=month
			#
			version=Tools.version_(path)
			line='{}: {}'.format(path,version)
			print(line)

	@staticmethod
	def version_(path_):
		'''version given a wks path'''
		version_h='{}/version/h/version.h'.format(path_)
		#print('version_h: {}'.format(version_h))
		if os.path.isfile(version_h):
			list_=['GM_MAJOR','GM_MINOR','GM_BUILD','GM_BUILD_STRING']
			with open(version_h) as f:
				for line in f:
					#print('line: {}'.format(line))
					if('#define' in line):
					#if line.startswith('#define'):
						if any(word in line for word in list_):
							items=line.split(' ')
							key=items[1]
							value=items[2].strip()
							#print('k,v: {} : {}'.format(key,value))
							if key=='GM_MAJOR':
								M=value
							elif key=='GM_MINOR':
								m_=value
							elif key=='GM_BUILD':
								b=value
							elif key=='GM_BUILD_STRING':
								bb=value
								break
				#print('version: {}.{}.{} {}'.format(M,m_,b,bb))
				return '{}.{}.{} {}'.format(M,m_,b,bb)
		else:
			return '0.0.0 0'

	@staticmethod
	def version(wks):
		'''given a wks path, prints: version: version value'''
		path_=Tools.path(wks)
		print('version: {}'.format(Tools.version_(path_)))

	@staticmethod
	def make(wks):
		'''build wks code'''
		try:
			import time
			from datetime import timedelta
			#
			#
			start = time.time()
			#
			#
			#
			path_=Tools.path(wks)
			#
			current_dir=os.getcwd()
			os.chdir(path_)
			print('current dir: {}'.format(path_))
			#
			#
			DEF.make_target='release'
			#
			p=Params('')
			p.use=wks
			#
			b=Build(p)
			b._Build__make()
			#
			#
			#
			os.chdir(current_dir)
			print('current dir: {}'.format(current_dir))
			#
			#
			end = time.time()
			elapsed=end-start
			print('elapsed time: {}'.format(elapsed))
			#
			#
			print(timedelta(seconds=elapsed))
			#
			#
			res=Build.res(wks)
			#
		except Exception as exception:
			show_error_details()

	@staticmethod
	def tests_list(wks):
		'''tests ids to choose which tests pass to test function as a parameters'''
		try:
			path_=Tools.path(wks)
			path__='{}/IntegrationTests/Tests'.format(path_)
			#
			import imp
			module = imp.load_source('func', '{}/func.py'.format(path__)) #'module.name'
			#
			for idx, val in enumerate(module._a):
				print( '{}: {}'.format(idx,val) )

		except Exception as exception:
			show_error_details()

	@staticmethod
	def test(wks,tests_list=''):
		'''run tests'''
		try:
			path_=Tools.path(wks)
			dir='{}/IntegrationTests/Tests'.format(path_)
			current_dir=os.getcwd()
			#
			os.chdir(dir)
			#
			t=tests_list.replace(',',' ')
			#
			cmd='. ./run_tests {}'.format(t) # 0 1, for example
			os.system(cmd)
			#
			os.chdir(current_dir)
		except Exception as exception:
			show_error_details()

	@staticmethod
	def transmissions():
		'''fixes transmissions folder issue, do use it when get code fails'''
		folder='/tmp/transmissions'
		group='g_epgm00'
		os.system('chgrp {} {}'.format(group, folder))
		os.system('chmod +rw {}'.format(folder))

	@staticmethod
	def third_party_versions():
		'''shows used third-party libs'''
		os.system('cat /etc/redhat-release')
		os.system('readlink /glb/data/geomechanics/software/diana')
		os.system('gcc --version | grep gcc')
		os.system('qmake --version | grep Qt')
		os.system('python --version')
		os.system('readlink $BOOST')

	@staticmethod
	def delete(wks):
		'''given a wks name delete code, build and drop folders and log file'''
		code=Tools.path(wks)
		build=Tools.build(wks)
		drop=Tools.drop_path(wks)
		#
		paths=[code,build,drop]		
		log='{}_{}'.format(build,DEF.logfile)
		#
		print('')
		print('--delete--')
		print(code)
		print(build)
		print(drop)
		print(log)
		print('')
		Tools.version(wks)
		print('')
		c=raw_input('delete?(y/[n])')
		print('')
		#
		if c=='y' or c=='yes':
			for path in paths:
				os.system('rm -rf {}'.format(path))	
			os.system('rm {}'.format(log))
			print('DELETED')
		else:
			print('NOT DELETED')
		#
		today = datetime.today()
		filter='{}/{}'.format(today.year,today.month)
		print('')	
		Tools.versions(filter)

	@staticmethod
	def modified(w):
		'''edited files in a given wks'''
		#gm_20201211_132201
		time='{}/{}/{} {}:{}:{}'.format(w[3:7],w[7:9],w[9:11], w[12:14],w[14:16],w[16:18])
		time_obj = datetime.strptime(time,'%Y/%m/%d %H:%M:%S')+timedelta(minutes=5)
		no_ext_but_Makefile='-o \( ! -name \"*.*\" -a ! -name \"Makefile\" \)'
		s=''
		for ext in ['h','cpp','pro','pri','py']:
			s+='-o -name \"*.{}\" '.format(ext)
		s1=s[3:]
		cmd='find {} -type f -size -200k \( {} {} \) -newermt \"{}\"'.format(Tools.path(w),s1,no_ext_but_Makefile,time_obj)
		#print(time)
		#print(cmd)
		os.system(cmd)

	@staticmethod
	def map(w):
		'''maps teh wks to work with tf cli'''
		cmd='tf workfold -map -workspace:{} -collection:{} "$/Geomec/{}" {}'.format(w,DEF.tf_collection,Tools.source(w),Tools.path(w))
		#print(cmd)
		os.system(cmd)
		#
		print('reference: http://www.tfstutorial.com/tf-command')
		print('')
		print('tf status')
		print('tf checkout file')
		print('tf add file')
		print('tf undo file')

	@staticmethod
	def module():
		'''module cmd usage and currently loaded modules'''
		print('')
		print('--usage examples--')
		print(' module --help')
		print(' module avail GCC')
		print(' module load GCCcore/7.3.0')
		print(' module show GCCcore/7.3.0')
		print(' modulelist')
		print('')
		print('--currenly loading--')

		files=['.bashrc','.profile']
		for file in files:
			path='{}/{}'.format(os.environ['HOME'],file)
			cmd='module load'
			if os.path.isfile(path):
				print(path+':')
				with open(path) as f:
					for line in f:
						if line.startswith(cmd):
							print(' '+line.strip()[len(cmd):])

	@staticmethod
	def last():
		'''current year last wks name'''
		today = datetime.today()
		paths=Tools.paths(today.year)
		name=''
		if paths:
			path=paths.pop()
			name=path.split('/')[-1]
		print('last:{}'.format(name))
		return name

	@staticmethod
	def envvar(val):
		a=os.environ[val]
		print('')
		print(a)
		print('')
		#
		aa=a.split(':')
		if len(aa) > 1:
			for i in aa:
				print(i)
			print('')

#######################################################################
#
#
#						main
#
#
#######################################################################
def main(argv):
	try:

		p=Params(argv)
		#
		# check wks exists 
		#
		need_wks=['make','inst','drop','wks','source_req','version','test','tests_list','res','delete','modified','map']
		if p.k in need_wks:
			if not Tools.exist(p.v):
				Params.usage()
				sys.exit(0)
		#
		# no param needed
		#
		options={
			'help':					Params.usage,
			'fix-transmissions':	Tools.transmissions,
			'last':					Tools.last,
			'third-party-versions':	Tools.third_party_versions,
			'module':				Tools.module }
		#
		# param needed
		#
		options_1={	
			#
			# need wks
			#
			'make':			Tools.make,
			'get':			Tools.get,
			'inst':			Tools.create_installation,
			'drop':			Tools.drop,
			'source_req':	Tools.source_req,
			'version':		Tools.version,
			'test':			Tools.test,
			'tests_list':	Tools.tests_list,
			'res':			Tools.res,
			'delete':		Tools.delete,
			'modified':		Tools.modified,
			'map':			Tools.map,
			#
			# need year or year/month
			#
			'list':			Tools.list,
			'versions':		Tools.versions,
			#
			# 
			#
			'envvar':		Tools.envvar }
		#
		if p.k in options or p.k in options_1:
			#
			# no params needed
			#
			if p.k in options:
				options[p.k]()
			#
			# need wks
			#
			elif p.k in options_1:
				options_1[p.k](p.v)
			#
			#
			#
		elif p.k in ['source','wks','default']: # p.use, p.source
			b=Build(p)
			b.run()
		else:
			Params.usage()
	except Exception as exception:
		show_error_details()

if __name__=='__main__':
	try:
		main( sys.argv[1:] )
	except Exception as exception:
		show_error_details()
