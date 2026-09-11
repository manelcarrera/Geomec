TEMPLATE = app
CONFIG += console

include(../../qmake/all.pri)
include(../../qmake/diana.pri)
include(../../qmake/libs.pri)

QT += xml

#5x
LIBS_TESTS=\
Fw51 \
TestLib \
SanityChecks \
ModelOperations \
Materials
for( lib, LIBS_TESTS ): LIBS += $${DESTDIR}/lib$$lib$$EXT

for( lib, LIBS_DIA ): LIBS += -L$(DIALIB) -l$$lib

LIBS += $${DESTDIR}/libgtest$$EXT

LIBS_ -= geomec_rgi_library
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

for( lib, LIBS_FW ): LIBS += $${DESTDIR}/lib$$lib$$EXT

unix: LIBS  += -L$(DIALIB) -l$$DLLLBGCROUTINES -l$$DLLLBGCCLASSES

#win32 {
#	LIBS += $(DIALIB)/$${DESTINATION}/$$DLLLBGCROUTINES$$LIBRARY_EXTENSION
#	LIBS += $(DIALIB)/$${DESTINATION}/$$DLLLBGCCLASSES$$LIBRARY_EXTENSION
#}


CONFIG(debug, debug|release):	LIBS   += -L$${DLLROOT}/debug -lRGInterface -lblosc -lModGMBus -lGMSkuaConnector
CONFIG(release, debug|release):	LIBS   += -L$${DLLROOT} -lRGInterface -lblosc -lModGMBus -lGMSkuaConnector

# MCR: don't move from here, I think needs to come after lRGInterface
LIBS += $${DESTDIR}/libRESCUE$$EXT

LIBS += -L$$(GM_SW_TOOL_PATH)/extra/lib -luuid

LIBS += -L$${LBROOT}/tbb/linux/lib/intel64/gcc4.7 -ltbb -ltbbmalloc

unix {
	LIBS  += -L$${GLIB2PATH}/lib -lglib-2.0
	LIBS  += -L/usr/lib64 -lz
	LIBS  += $${LBROOT}/hdf5/linux/lib/libhdf5_cpp.a $${LBROOT}/hdf5/linux/lib/libhdf5.a
	LIBS  += -ldl -lrt
	LIBS += -L$(BOOST)/lib -lboost_date_time
}

PRE_TARGETDEPS += $${DESTDIR}

# Clean

unix {
	QMAKE_DISTCLEAN += fgvstr.str geomec.dat geomec.ini MyDoc.dat
	QMAKE_DISTCLEAN += test test_output.trx test-debug
	QMAKE_DISTCLEAN += debug/Tests.vcxprojResolveAssemblyReference.cache
	QMAKE_DISTCLEAN += $${TESTSROOT}/Models/Fw51Output/*/*
}

#
# default DESTDIR is for static libs
#
DESTDIR = $$DESTDIR_


# Unit test
# release

test.target   = test
#test.depends  = $${DESTINATION}/$${TARGET} run_tests

unix:test.commands = \
cd ../../test/Tests && \
/bin/sh ./run_tests \
$$CONF \
--gm-model-path=../../Tests/Models && \
touch test && \
cd -
	
#win32:test.commands = $${DESTINATION}/$${TARGET} --gm-model-path=$${TESTSROOT}/Models && touch test


QMAKE_EXTRA_TARGETS += test


# debug
kk {
	test-debug.target   = test-debug
	
	unix { 
		test-debug.depends  = ./debug/$${TARGET} run_tests
		test-debug.commands = /bin/sh ./run_tests debug --gm-model-path=$${TESTSROOT}/Models && touch test-debug
	}
	
	win32 {
		test-debug.depends  = ./Debug/$${TARGET}
		test-debug.commands = ./Debug/$${TARGET} --gm-model-path=$${TESTSROOT}/Models && touch test-debug
	}

	QMAKE_EXTRA_TARGETS += test-debug
}
