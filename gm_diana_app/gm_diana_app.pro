TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
include(../qmake/libs.pri)

QT += widgets xml	# QApplication 

for( lib, LIBS_DIA ): LIBS += -L$(DIALIB) -l$$lib

#
# order matters !!! 
#
LIBS_=\
DianaStartup \
DianaCore \
QUtil \
Util
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

unix {
	LIBS += -lrt 
	LIBS += -L$(BOOST)/lib -lboost_date_time
}

PRE_TARGETDEPS += $${DESTDIR}

#
# default DESTDIR is for static libs
#
DESTDIR = $$DESTDIR_

