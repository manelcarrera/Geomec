# This won't work by now as windows 'gm42.exe binary' is called

TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
include(../qmake/libs.pri)

QT += widgets xml	# QApplication 

for( lib, LIBS_DIA ): LIBS += -L$(DIALIB) -l$$lib

LIBS_=\
QUtil \
Util \
DianaStartup \
DianaCore \
DianaSignals
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

LIBS_FW=\
Geometry \
Tensor
for( lib, LIBS_FW ): LIBS += $${DESTDIR}/lib$$lib$$EXT

CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/debug -lModGMBus
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lModGMBus