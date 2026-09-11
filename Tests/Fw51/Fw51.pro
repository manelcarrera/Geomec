TEMPLATE = lib
CONFIG += staticlib
QT += widgets # QApplication
ROOT__=../..
include($$ROOT__/qmake/all.pri)
#unix:QMAKE_DISTCLEAN += $$files(../Models/Fw51Output/*) 	# $$files usage sample
