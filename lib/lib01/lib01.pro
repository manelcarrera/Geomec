include(../../app/app01/common.pri) # LIBDIR
#
TEMPLATE = lib
#
CONFIG += staticlib
#
INCLUDEPATH = h
#
SOURCES = src/lib01.cpp
#
DESTDIR = $$LIBSDIR