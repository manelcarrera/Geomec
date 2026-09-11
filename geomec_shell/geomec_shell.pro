TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
include(../qmake/libs.pri)

QT += xml

for( lib, LIBS_DIA ):	LIBS += -L$(DIALIB) -l$$lib
for( lib, LIBS_ ):		LIBS += $${DESTDIR}/lib$$lib$$EXT
for( lib, LIBS_FW ):	LIBS += $${DESTDIR}/lib$$lib$$EXT

unix: LIBS  += -L$(DIALIB) -l$$DLLLBGCROUTINES -l$$DLLLBGCCLASSES

#win32 {
#	LIBS += $(DIALIB)/$${DESTINATION}/$$DLLLBGCROUTINES$$LIBRARY_EXTENSION
#	LIBS += $(DIALIB)/$${DESTINATION}/$$DLLLBGCCLASSES$$LIBRARY_EXTENSION
#}

# MCR: Rescue needs to go after RGInterface, so in this case order matters
CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/debug -lRGInterface -lModGMBus -lGMSkuaConnector
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lRGInterface -lModGMBus -lGMSkuaConnector

LIBS += $${DESTDIR}/libRESCUE$$EXT

CONFIG(debug, debug|release):	LIBS   += -L$${DLLROOT}/debug -lblosc
CONFIG(release, debug|release):	LIBS   += -L$${DLLROOT} -lblosc

LIBS += -L $${LBROOT}/tbb/linux/lib/intel64/gcc4.7 -ltbb -ltbbmalloc

unix {
	LIBS  += -L$${GLIB2PATH}/lib -lglib-2.0
	
	LIBS  += -L/usr/lib64 -lz
	LIBS  += $${LBROOT}/hdf5/linux/lib/libhdf5_cpp.a $${LBROOT}/hdf5/linux/lib/libhdf5.a
	LIBS  += -ldl -lrt
	LIBS += -L$(BOOST)/lib -lboost_date_time
}

message( $$LIBS )

PRE_TARGETDEPS += $${DESTDIR}

#
# default DESTDIR is for static libs
#
DESTDIR = $$DESTDIR_

# Unit test

test.target = test
#test.depends = $$files($${TESTSROOT}/Models/CORA/*) $${DESTDIR}/$${TARGET} run_geomec_shell test_geomec_shell

#test.commands = \
#cd ../test/geomec_shell && \
#$(SHELL) \
#test_geomec_shell \
#test_output \
#$${CONF} && \
#touch test && \
#cd -

test.commands = \
cd ../test/geomec_shell && \
python test_shell.py && \
touch test && \
cd -

QMAKE_EXTRA_TARGETS += test

