TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)

CONFIG(debug, debug|release):	LIBS += -L$${DLLROOT}/debug -lRGInterface
CONFIG(release, debug|release):	LIBS += -L$${DLLROOT} -lRGInterface

LIBS += $${DESTDIR}/libRESCUE$$EXT
