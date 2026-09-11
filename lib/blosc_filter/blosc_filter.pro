TEMPLATE = lib
CONFIG += staticlib
include(../../qmake/all.pri)
SOURCES = $$MY_PATH/src/*.c # override SOURCES var because there are not *.cpp files, only *.c