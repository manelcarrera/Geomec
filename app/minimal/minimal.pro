# This won't work by now as windows 'gm42.exe binary' is called

TEMPLATE = app
CONFIG += console release

QMAKE_CXXFLAGS += -std=c++14
#QMAKE_CXXFLAGS += -Wl,-M

DIANA += $(DIAROOT)/share/src/include
DIANA += $(DIAROOT)/release/src/include

INCLUDEPATH = $$DIANA
INCLUDEPATH += $(QTDIR)/QtCore
INCLUDEPATH += $(DIAPATH)/share/src/lb/gc51/classes/include

SOURCES = main.cpp

#LIBS  = -L$(DIAPATH)/release/lib -llbgc51classes
LIBS  = -L./ -llbgc51classes

message( LIBS=$$LIBS )