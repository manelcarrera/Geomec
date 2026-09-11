CONF=release
CONFIG += console $$CONF
#
QMAKE_CXXFLAGS += -std=c++14
#
MY_PATH=$$(PWD)
SPLIT=$$split( MY_PATH, / ) # not Ok in windows
a_=$$member(SPLIT,0,3)
BUILD=/$$join(a_,/)_build
#
DESTDIR = $$BUILD/bin/$$CONF	# app
LIBSDIR = $$DESTDIR/.a  		# lib
#
OBJECTS_DIR = $${BUILD}/.obj/$${CONF}/$${FILENAME}
