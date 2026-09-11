TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
include(../qmake/libs.pri)
QT += xml # needed

#diana
for( lib, LIBS_DIA ): LIBS += -L$(DIALIB) -l$$lib

# own libs
LIBS_ -= geomec_rgi_library
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

# fw libs
for( lib, LIBS_FW ): LIBS += $${DESTDIR}/lib$$lib$$EXT


unix: LIBS  += -L$(DIALIB) -l$$DLLLBGCROUTINES -l$$DLLLBGCCLASSES

#win32 {
#	LIBS += $$DIALIB/$${DESTINATION}/$$DLLLBGCROUTINES$$LIBRARY_EXTENSION
#	LIBS += $$DIALIB/$${DESTINATION}/$$DLLLBGCCLASSES$$LIBRARY_EXTENSION
#}

CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/debug -lModGMBus -lblosc -lGMSkuaConnector
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lModGMBus -lblosc -lGMSkuaConnector

LIBS += -L $${LBROOT}/tbb/linux/lib/intel64/gcc4.7 -ltbb -ltbbmalloc

unix {
	LIBS  += -L$${GLIB2PATH}/lib -lglib-2.0
	LIBS  += -L/usr/lib64 -lz
	LIBS  += $${LBROOT}/hdf5/linux/lib/libhdf5_cpp.a $${LBROOT}/hdf5/linux/lib/libhdf5.a
	LIBS  += -ldl -lrt
}

#message( $$LIBS )

# Unit test
# release
test.commands = cd ../../test/geomec_cora && python ./test_cora.py && cd -

#message( test.commands = \"$$test.commands\" )

QMAKE_EXTRA_TARGETS += test


# win32 : it was not implemented