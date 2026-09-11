QT += core gui widgets xml network
TEMPLATE = lib

include(../../../qmake/all.pri)

# MCR: Create 'DESTDIR' after including 'all.pri'
CONFIG(debug, debug|release):  	DESTDIR = $${DLLROOT}/$$DESTINATION
CONFIG(release, debug|release):	DESTDIR = $${DLLROOT}