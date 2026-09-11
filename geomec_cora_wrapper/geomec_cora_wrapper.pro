TEMPLATE = app
CONFIG += console
ROOT__=..
include($$ROOT__/qmake/all.pri)
TARGET = geomec_cora
PRE_TARGETDEPS += $${DESTDIR}
DESTDIR = $$DESTDIR_