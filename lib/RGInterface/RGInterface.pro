TEMPLATE = lib
CONFIG += plugin

DEFINES += RGINTERFACE_EXPORT

include(../../qmake/all.pri) # overrides 'DESTDIR'? Yes, in 'destination.pri'

# MCR: Create 'DESTDIR' after including 'all.pri'
CONFIG(debug, debug|release):  	DESTDIR = $${DLLROOT}/$$DESTINATION
CONFIG(release, debug|release):	DESTDIR = $${DLLROOT}