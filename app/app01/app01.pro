include(common.pri) # DESTDIR
#
TEMPLATE = app
#
QT -= gui
QT -= core
#
INCLUDEPATH = h 
INCLUDEPATH += ../../lib/lib01/h
#
SOURCES = src/main.cpp
#
# dyn
#LIBS  = -L$$LIBSDIR -llib01
# static
LIBS = $$LIBSDIR/liblib01.a
#
PRE_TARGETDEPS += $$LIBSDIR # -> needed, otehrwise lib is compiled but app is not updated with the new lib