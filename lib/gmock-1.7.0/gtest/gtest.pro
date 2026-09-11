# now 'gtest/gtest.pro'; befoe it was 'gtest/qmake/qmake.pro'
TEMPLATE = lib
CONFIG += staticlib
include(../../../qmake/all.pri)
SOURCES = $$MY_PATH/src/*.cc
HEADERS = $$MY_PATH/include/gtest/*.h
HEADERS += $$MY_PATH/include/gtest/internal/*.h
# no qmake/includepath/GTEST.pri
INCLUDEPATH = $$MY_PATH/include
