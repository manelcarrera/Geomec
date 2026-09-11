# This won't work by now as windows 'gm42.exe binary' is called

TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
#QT += xml # needed

LIBS_=\
QUtil \
Util
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$LIBRARY_EXTENSION

CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/$${DESTINATION} -lModGMBus
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lModGMBus
