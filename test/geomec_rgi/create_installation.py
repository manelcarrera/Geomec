#!/usr/bin/env python

from shutil import copy2, rmtree, copytree, move
import os
import sys
import tarfile
import logging
import ntpath
import datetime
from glob import glob


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

#
# windows: many adaptations needed, on 2020-11 not-completed
#
class CreateInstallation:
	def __init__(self):
		try:
			if sys.platform != 'win32':
				self.__check_envvars() # if not present sys.exit()

			if sys.platform == 'win32':
				self.__get_envvars_windows()
			else:
				self.__get_envvars()
			#
			# set members
			#
			
			#a='/scratch/2020/11/gm_20201113_132042/test/geomec_rgi'
			a=os.path.dirname(os.path.abspath(__file__))
			b=a.split('/') # '','scratch','2020','11','gm_20201113_132042','test','geomec_rgi'
			#
			# a) ROOT
			#
			c=b[1:5] 
			d='/'+'/'.join(c)
			#
			# b) BUILD
			#
			dd='/{}/{}/{}/__build__/{}'.format(b[1],b[2],b[3],b[4])

			self.ROOT=d # the one above was also Ok
			print('ROOT: {}'.format(self.ROOT))

			self.BUILD=dd
			print('BUILD: {}'.format(self.BUILD))

			self.TMP='geomec_rgi_TMP'
			self.DIANA_DEST='{}/diana'.format(self.TMP)
			self.ZIP='geomec_rgi.tar.bz2'
			#
			self.version='0.0.0' # init to a wrong value
			#
			# dest of tar.bz2 file
			# IFF: Install Files Folder
			# IFP: Install Files Path
			self.IFF='install_files' 
			self.IFP='{}/bin/{}/{}'.format(self.BUILD,self.CONF,self.IFF)
			#
		except Exception as exception:
			show_error_details()
	#
	# check env vars
	#
	def __check_envvars(self):
		envvars=['DIAPATH','DIASRC','DIAINTEL',
			'QTLIB',
			'SHELLSOFTBASE',
			'CONF',
			'BOOST'] 
		for envvar in envvars:
			if not os.getenv(envvar):
				print('{}: not defined'.format(envvar))
				sys.exit(1)
	#
	# get env vars
	#
	def __get_envvars(self):
		self.CONF=os.getenv('CONF')
		self.DIAPATH=os.getenv('DIAPATH') #DIAPATH=$BASEPATH/diana/$CONF
		self.DIASRC=os.getenv('DIASRC')
		self.DIAINTEL=os.getenv('DIAINTEL')
		self.QTLIB=os.getenv('QTLIB')
		self.SHELLSOFTBASE=os.getenv('SHELLSOFTBASE')
		self.BOOST=os.getenv('BOOST')

	def __get_envvars_windows(self):
		self.CONF='release'
		#
		self.DIAROOT='C:/aWork/aDiana/diana_CURRENT_COMPILE'	# '\a' behaves wrong
		#
		self.DIAPATH='{}/release'.format(self.DIAROOT)
		self.DIASRC='{}/share/src'.format(self.DIAROOT)
		self.DIAINTEL='{}/share/Intel'.format(self.DIAROOT)
		#
		self.QTLIB='{}/lib'.format( os.getenv('QTDIR') )
		#
		# skipped by now
		#
		#self.SHELLSOFTBASE=os.getenv('SHELLSOFTBASE')
	#
	# makedirs in 'geomec_rgi_TMP/diana'
	#
	def __makedirs_TMP(self):
		DIRS=[
			'bin',
			'binseg',
			'binseg/ap',
			'lib',
			'hasp',
			'intel',
			'share',
			'share/lib',
			#'share/lib/ElmTyp', #it must not exist to run copytree later
			'share/src',
			'share/src/ap',
			'share/src/dc',
			'share/src/in',
			'share/src/lb',
			'share/src/ls',
			'share/src/nl',
			'share/src/pf',
			'share/src/po',
			'share/src/so'
		]

		for dir in DIRS:
			#os.makedirs('{}/{}'.format(DIANA_DEST,dir), True) #exist_ok ... but not needed ... it fails
			os.makedirs('{}/{}'.format(self.DIANA_DEST,dir)) 
	#
	# copy binseg
	#
	def __copy_binseg(self):
		try:
			APPS=[
				'ca40',
				'gm42',
				'ho40',
				'in51',
				'mc41'
			]

			if sys.platform == 'win32':
				APPS[:] = [x + '.exe' for x in APPS]

			# FIXME
			#copy2(	'{}/hasp/libhasp_linux_x86_64_3659.so'.format(DIAPATH), 
			#		'{}/hasp'.format(DIANA_DEST))
			#sys.exit(0)


			# create apps folders
			#APPDEST='{}/binseg/ap'.format(DIANA_DEST)
			# no need, already done above
			#os.makedirs(APPDEST)

			# copy: 'binseg/ap/*'
			for app in APPS:
				copy2(	'{}/binseg/ap/{}'.format(self.DIAPATH,app),
						'{}/binseg/ap'.format(self.DIANA_DEST) )
		except Exception as exception:
			show_error_details()
	#
	# 3) copy: 'bin/*'
	#
	def __copy_bin(self):
		
		files = glob('{}/bin/*'.format(self.DIAPATH))
		for file in files:
			copy2(file, '{}/bin'.format(self.DIANA_DEST))
	#
	# copy libs
	#
	def __copy_libs(self):
		LIBS=[
			'SCHEMA',
			'SCHEMA.bin',
			'ElmTypes.dat'#,
			#'*.so' # ?????????
		]
		# copy 'lib/some_files'
		for lib in LIBS:
			copy2(	'{}/lib/{}'.format(self.DIAPATH,lib), 
					'{}/lib'.format(self.DIANA_DEST))

		# copy: 'lib/*.so'
		so_files = glob('{}/lib/*.so'.format(self.DIAPATH))
		for file in so_files:
			copy2(file, '{}/lib'.format(self.DIANA_DEST))
	#
	# copy share
	#
	def __copy_share(self):

		BASEPATH=self.DIAPATH
		if sys.platform == 'win32':
			BASEPATH=self.DIAROOT 

		SRCLIBS=[
			'DI_ERROR.DAT',
			'femsys.ini',
			'filoscap',
			'geoclass.res',
			'matclass.res'#,
			#'ElmTyp/*'		#?????????
		]
		# copy 'share/lib/some_files'
		for lib in SRCLIBS:
			copy2(	'{}/share/lib/{}'.format(BASEPATH,lib), 
					'{}/share/lib'.format(self.DIANA_DEST))

		# copy 'share/lib/ElmTyp' entire folder
		copytree(	'{}/share/lib/ElmTyp'.format(BASEPATH,lib), 
					'{}/share/lib/ElmTyp'.format(self.DIANA_DEST))
	#
	# copy share
	#
	def __copy_keyfile(self):
		copy2(	'{}/geomec/keyfile/Linux/diafgv.k72'.format(self.ROOT), 
				'{}/share/lib'.format(self.DIANA_DEST))
	#
	# copy txt
	#
	# copy entire folders 'share/src/some_folders'
	#
	def __copy_txt(self):
		# folders
		TXTS=[
			'ap',
			'dc',
			'in',
			'lb',
			'ls',
			'nl',
			'pf',
			'po',
			'so'
		]
		for folder in TXTS:
			files = glob('{}/{}/*.txt'.format(self.DIASRC, folder))
			for file in files:
				copy2(file, '{}/share/src/{}'.format(self.DIANA_DEST,folder))
	#
	# copy hasp
	#
	# copy: hasp lib: same version from 2014-10-27
	# lib not distributed by error in release 5.7.0.6
	#
	def __copy_hasp(self):
		#
		# FIXME: to be implemented for windows ???
		#
		if sys.platform == 'win32':
			return
		copy2(	'{}/hasp/libhasp_linux_x86_64_3659.so'.format(self.DIAPATH), 
				'{}/hasp'.format(self.DIANA_DEST))
	#
	# copy usr dlls
	#
	# FIXME: to be verified !!!
	#
	# copy UsrDLLs/* (but 'debug' folder in it)
	#
	def __copy_usrdll_OLD(self):
		copytree(	'{}/UserDLLs'.format(self.ROOT),
					'{}/UserDLLs'.format(self.TMP))
		rmtree( '{}/UserDLLs/debug'.format(self.TMP), True ) #just in case

	def __copy_usrdll(self):
		copytree(	'{}/bin/{}/so'.format(self.BUILD,self.CONF),
					'{}/UserDLLs'.format(self.TMP))
		rmtree( '{}/UserDLLs/debug'.format(self.TMP), True ) #just in case
	#
	# copy scripts
	#
	def __copy_scripts(self):
		for script in ['run_geomec_rgi','test_geomec_rgi']:
			copy2(	'{}_INSTALLER'.format(script), 
					'{}/{}'.format(self.TMP,script))

	#
	# copy generated bins: geomec_shell and geomec_rgi
	#
	def __copy_generated_bins_OLD(self):
		#copy2('{}/bin/{}/geomec_rgi'.format(ROOT,CONF), TMP)
		copy2('{}/bin/{}/geomec_shell'.format(self.ROOT,self.CONF), self.TMP)

	def __copy_generated_bins(self):
		copy2('{}/bin/{}/geomec_shell'.format(self.BUILD,self.CONF), self.TMP)
		copy2('{}/bin/{}/gm_diana_app'.format(self.BUILD,self.CONF), self.TMP)
	#
	# copy test models
	#
	def __copy_test_models(self):
		# copy models nedded to run the tests
		copytree(	'{}/Tests/Models/RGI/D0'.format(self.ROOT), 
					'{}/D0'.format(self.TMP))

		# TODO: verify later
		# delete svn files and folders 
		try:
			rmtree('{}/D0/.svn'.format(self.TMP),True)  #verify
		except Exception as exception:
			show_error_details()
	#
	# make third party dirs
	#
	def __makedirs_3rd_party_libs(self):
		DIRS=[
			'Qt',
			'gcc/lib64',
			'tbb',
			'glib',
			'png',
			'zlib',
			'boost'
			#'GCC', # crec que no caldria
		]
		for dir in DIRS:
			os.makedirs('{}/{}'.format(self.TMP,dir))
	#
	# copy intel libs
	#
	def __copy_intel_libs(self):
		for pattern in ['*.so','*.so.*']:
			so_files = glob('{}/{}'.format(self.DIAINTEL,pattern))
			for file in so_files:
				copy2(file, '{}/intel'.format(self.DIANA_DEST))

	#
	# copy tbb libs
	#
	def __copy_tbb_libs(self):
		TBBPATH='{}/lib/tbb/linux/lib/intel64/gcc4.7'.format(self.ROOT)
		for lib in ['tbb','tbbmalloc']:
			so_files = glob('{}/lib{}.so*'.format(TBBPATH,lib))
			for file in so_files:
				copy2(file, '{}/tbb'.format(self.TMP))

	#
	# copy BOOST libs
	#
	def __copy_boost_libs(self):
		so_files = glob('{}/lib/libboost_date_time.so*'.format(self.BOOST))
		for file in so_files:
			copy2(file, '{}/boost'.format(self.TMP))
	#
	# copy qt libs
	#
	def __copy_qt_libs(self):
		QTLIBS=[
			'Core',
			'Gui',
			'Widgets',
			'Network',
			'Xml'
		]
		for lib in QTLIBS:
			so_files = glob('{}/libQt5{}.so*'.format(self.QTLIB,lib))
			for file in so_files:
				copy2(file, '{}/Qt'.format(self.TMP))

	#
	# copy system libs
	#
	def __copy_system_libs(self):
		LIBPNG='{}/libpng/1.6.34-GCCcore-7.3.0/lib'.format(self.SHELLSOFTBASE)
		GLIB='{}/GLib/2.54.3-GCCcore-7.3.0/lib'.format(self.SHELLSOFTBASE)
		ZLIB='{}/zlib/1.2.11-GCCcore-7.3.0/lib'.format(self.SHELLSOFTBASE)
		GCCLIB64='{}/GCCcore/7.3.0/lib64'.format(self.SHELLSOFTBASE)

		LIBS=[
			[GCCLIB64,'gcc/lib64'],
			[LIBPNG,'png'],
			[GLIB,'glib'],
			[ZLIB,'zlib']
		]
		# 12e) copy: some system libs
		for lib in LIBS:
			for pattern in ['*.so','*.so.*']:
				so_files = glob('{}/{}'.format(lib[0],pattern))
				for file in so_files:
					copy2(file, '{}/{}'.format(self.TMP,lib[1]))

	#
	# get version
	#
	# geomec_rgi -> f:['**', 'RockMech', 'Processor', '5.7.0.7', 'linux', 'build', '19327.1']
	#
	def __version(self):
		try:
			CMD="{}/geomec_rgi --version".format(self.TMP)
			version_=os.popen(CMD).read()

			f = version_.split('\n', 1)[0].split(' ')
			f=f[3:] # geomec_rgi
			self.version=' '.join(f)

			print('Version: {}'.format(self.version))
		except Exception as exception:
			show_error_details()

	#
	# geomec_shell -> f:['geomec_shell', '5.7.0.7', 'linux', 'build', '19327.1']
	#
	def __version_gm_shell(self):
		try:
			CMD="LD_LIBRARY_PATH=$LD_LIBRARY_PATH:{}/UserDLLs {}/geomec_shell --version".format(self.TMP,self.TMP)
			version_=os.popen(CMD).read()

			f = version_.split('\n', 1)[0].split(' ')
			f=f[1:] # geomec_shell
			self.version=' '.join(f)

			print('Version: {}'.format(self.version))
		except Exception as exception:
			show_error_details()
	#
	# tar
	#
	def __tar(self):
		now=datetime.datetime.now()
		date_s = now.strftime('%H:%M:%S')

		print('Zipping \'{}\' ... takes 2 minutes ... {}'.format(self.ZIP,date_s))

		# not nice but it's what it is
		CMD='cd {} && tar -cjf ../{} *'.format(self.TMP,self.ZIP)
		os.system( CMD )

		# system call is twice faster ans more flexible
		# verify : its also copying _TMP folder instead of just its contents
		#with tarfile.open('geomec_rgi.tar.bz2', "w:bz2") as tar:
		#	tar.add(TMP, arcname=os.path.basename(TMP))
	#
	# move files to installation folder path (IFP)
	#
	def __move_files_to_IFP(self):
		try:
			#FIXME
			#rmtree(TMP)

			# dest of tar.bz2 file
			# IFF: Install Files Folder
			# IFP: Install Files Path
			current_dir=os.getcwd()
			print('current_dir: {}'.format(current_dir))
			print('Zip moved to \'{}\''.format(self.IFP))

			# remove temporary folder needed to tar files and folders 
			#print(INSTALL_FILES)
			rmtree(self.IFP,True) #ignore_errors
			os.makedirs(self.IFP)

			# move tar file
			move(self.ZIP,self.IFP)

			CMD='ls -lh {}/{}'.format(self.IFP,self.ZIP)
			os.system(CMD)

			FILES=[
				'README.install',
				'install'
			]
			# 13d) move help files to final folder
			for file in FILES:
				#print('copy: {} -> {}'.format(file,self.IFP))
				copy2(file,self.IFP)

			# 13e) genarate 'VERSION' file
			FILE='{}/VERSION'.format(self.IFP)
			with open(FILE, "w") as file:
				file.write(self.version)
		except Exception as exception:
			show_error_details()

	def run(self):
		try:
			print('create installation >>')
			#
			# FIXME
			rmtree( self.TMP, True ) #ignore_errors 
			#os.system('rm -rf {}'.format(TMP))

			self.__makedirs_TMP()
			self.__copy_binseg()
			self.__copy_bin()
			self.__copy_libs()
			self.__copy_keyfile()
			self.__copy_share()
			self.__copy_txt()
			self.__copy_hasp()
			self.__copy_usrdll()
			#self.__copy_scripts()			# removed on 2020-11, 'run_geomec_rgi' and 'test_geomec_rgi'
			self.__copy_generated_bins()	# 2x: geomec_shell and gm_diana_app
			#self.__copy_test_models()		# removed on 2020-11, D0 dir
			self.__makedirs_3rd_party_libs()# Qt, gcc/lib64, tbb, glib, png, zlib
			self.__copy_intel_libs()
			self.__copy_tbb_libs()
			self.__copy_boost_libs()
			self.__copy_qt_libs()
			self.__copy_system_libs()
			#
			self.__tar()					# geomec_rgi.tar.bz2
			self.__version_gm_shell()		# somethng like '5.7.0.7 linux build 19327.1'
			self.__move_files_to_IFP()		# -> bin/release/install_files
			#
			print('create installation <<')
			#
		except Exception as exception:
			show_error_details()

#######################################################################
#
#
#						Tests
#
#
#######################################################################
class Tests:
	#
	# 
	#
	@staticmethod
	def test_01():
		'''create tar and move it to gm_DATE_TIME_build/bin/CONF/install_files'''
		ci = CreateInstallation()
		#
		ci._CreateInstallation__tar()
		ci._CreateInstallation__version_gm_shell()
		ci._CreateInstallation__move_files_to_IFP()

	@staticmethod
	def test_02():
		ci = CreateInstallation()
		ci._CreateInstallation__version_gm_shell()
		print('vesion:{}'.format(ci.version))

def main( argv ):
	try:
		ci = CreateInstallation()
		return
		ci.run()
		#
		#
		#
		#Tests.test_01()
		#
	except Exception as exception:
		show_error_details()


if __name__=='__main__':
	try:
		main( sys.argv[1:] )
	except Exception as exception:
		show_error_details()