# This won't work by now as windows 'gm42.exe binary' is called

TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
QT += xml # needed

LIBS_=\
ConsoleLibrary \
GeomecStringTable \
QUtil \
Util
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/debug -lModGMBus
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lModGMBus
