target=lib

#
# 1) CONF
#
CONFIG(debug, debug|release) : CONF=debug
CONFIG(release, debug|release) : CONF=release


#
# 2) MY_PATH
#
MY_PATH=$$(PWD) # $$(PWD) != $$PWD

message( globals.pri : MY_PATH:$$MY_PATH ) 

SPLIT=$$split( MY_PATH, / )
#message( SPLIT:$$SPLIT )

#
# 3a) BUILD : /scratch/YEAR/MONTH/gm_DATE_TIME_build
#
#a_=$$member(SPLIT,0,3) # 
#BUILD=/$$join(a_,/)_build
#
# 3b) BUILD : /scratch/YEAR/MONTH/__build__/gm_DATE_TIME
#
a_=$$member(SPLIT,0,2) # scratch 2020 12 gm_DATE_TIME
BUILD=/$$join(a_,/)/__build__/$$member(SPLIT,3)

#message( BUILD:$$BUILD )

#
# 4) TYPE
#
TYPE=bin
contains( SPLIT, 'lib' ): TYPE=lib
contains( SPLIT, 'fw51' ): TYPE=fw51
#contains( SPLIT, 'Tests' ): TYPE=Tests
contains( SPLIT, 'Tests' ) { 
	TYPE=Tests
	contains( SPLIT, 'lib' ): TYPE=Tests-lib
}
contains( SPLIT, 'Prototypes' ): TYPE=Proto

message( TYPE:$$TYPE )

#
# 5) ROOT
#
equals( TYPE, 'bin' ): ROOT = .. 
else: ROOT = ../..
#
# 5b) ROOT: exceptions
#
contains( SPLIT, 'gtest' ):	ROOT = ../../..
contains( SPLIT, 'Prototypes' ): ROOT = ../../..
equals( TYPE, 'Tests-lib' ): ROOT = ../../..

#
# 6) more specific ROOT paths
#
GMROOT				= $$ROOT/geomec
GEOMEC_PATH			= $$GMROOT
LBROOT				= $$ROOT/lib
FWROOT				= $$ROOT/fw51
TESTSROOT			= $$ROOT/Tests
#TESTSLIBROOT		= $$ROOT/Tests/lib
TESTSLIBROOT		= $$TESTSROOT
APPROOT				= $$ROOT/app
DLLROOT				= $$BUILD/bin/$${CONF}/so
PROTOROOT			= $$ROOT/Prototypes/ModularGeomec
#PYTHONROOT			= /usr/bin/python
PYTHONROOT			= /glb/data/geomechanics/software/PYTHON/python_it_distro #FIXME

#
# To make paths shorter for self path
# Do it better later when INCLUDEPATH has been filled
#
equals( TYPE, 'lib' ): LBROOT=..
equals( TYPE, 'fw51' ): FWROOT=..
equals( TYPE, 'Tests' ) { 
	TESTSROOT=..
	TESTSLIBROOT=..
}
equals( TYPE, 'Tests-lib' ): TESTSLIBROOT=..
equals( TYPE, 'Proto' ): PROTOROOT=..

contains( SPLIT, 'gtest' ): LBROOT=../.. #exception

#contains( SPLIT, 'DianaGlue' ): TESTSROOT=../.. # FIXME

# DIAROOT : ENVVAR : export DIAROOT=/glb/data/geomechanics/software/diana

message( globals.pri : ROOT:$$ROOT ) 
message( globals.pri : DLLROOT:$$DLLROOT ) 
message( globals.pri : CONF:$${CONF} ) 

#message( TESTSROOT:$$TESTSROOT )

logs {
	message(----------------) 
	message(GMROOT:$$GMROOT) 
	message(GEOMEC_PATH:$$GEOMEC_PATH) 
	message(LBROOT:$$LBROOT) 
	message(FWROOT:$$FWROOT) 
	message(TESTSROOT:$$TESTSROOT) 
	message(TESTSLIBROOT:$$TESTSLIBROOT) 
	message(GEOMECMODEL_PATH:$$GEOMECMODEL_PATH) 
	message(DLLROOT:$$DLLROOT) 
	message(PYTHONROOT:$$PYTHONROOT) 
	message(PROTOROOT:$$PROTOROOT) 
}