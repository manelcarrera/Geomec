# This won't work by now as windows 'gm42.exe binary' is called

TEMPLATE = app
CONFIG += console release

QT += core xml

QMAKE_CXXFLAGS += -std=c++14

DESTDIR = ./

ROOT = ..
LBROOT = $$ROOT/lib

# QUtil
INCLUDEPATH += $$LBROOT/QUtil/h
INCLUDEPATH += $$LBROOT/Util/h
INCLUDEPATH += $$LBROOT/ConsoleLibrary/h
INCLUDEPATH += $$LBROOT/GeomecStringTable/h

SOURCES = main.cpp

LIBS_=\
ConsoleLibrary \
GeomecStringTable \
QUtil \
Util
for( lib, LIBS_ ): LIBS += ../bin/release/lib$${lib}.a

message( INCLUDEPATH=$$INCLUDEPATH )
message( LIBS=$$LIBS )