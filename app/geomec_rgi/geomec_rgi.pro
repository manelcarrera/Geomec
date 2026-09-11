TEMPLATE = app
CONFIG += console
include(../qmake/all.pri)
include(../qmake/diana.pri) 				# . ./create.diana.pro
include(../qmake/libs.pri)
QT += xml # needed

for( lib, LIBS_DIA ): LIBS += -L$(DIALIB) -l$$lib

LIBS_ -= geomec_cora_library
for( lib, LIBS_ ): LIBS += $${DESTDIR}/lib$$lib$$EXT

for( lib, LIBS_FW ): LIBS += $${DESTDIR}/lib$$lib$$EXT

# MCR: Rescue needs to go after RGInterface, so in this case order matters
CONFIG(debug, debug|release):	LIBS       += -L$${DLLROOT}/debug -lRGInterface -lModGMBus -lGMSkuaConnector
CONFIG(release, debug|release):	LIBS       += -L$${DLLROOT} -lRGInterface -lModGMBus -lGMSkuaConnector

LIBS += $${DESTDIR}/libRESCUE$$EXT

CONFIG(debug, debug|release): 	LIBS += -L$${DLLROOT}/debug -lblosc
CONFIG(release, debug|release):	LIBS += -L$${DLLROOT} -lblosc

LIBS += -L $${LBROOT}/tbb/linux/lib/intel64/gcc4.7 -ltbb -ltbbmalloc

unix: LIBS  += -L$(DIALIB) -l$$DLLLBGCROUTINES -l$$DLLLBGCCLASSES

unix {
	LIBS  += -L/usr/lib64 -lz
	LIBS  += -L$${GLIB2PATH}/lib -lglib-2.0
	LIBS  += $${LBROOT}/hdf5/linux/lib/libhdf5_cpp.a $${LBROOT}/hdf5/linux/lib/libhdf5.a
	LIBS  += -ldl -lrt
}

#win32 {
#	LIBS += $$FWLIB/$${DESTINATION}/$$DLLLBGCROUTINES$$LIBRARY_EXTENSION
#	LIBS += $$FWLIB/$${DESTINATION}/$$DLLLBGCCLASSES$$LIBRARY_EXTENSION
#}



# Unit test

# release
test.commands = \
cd ../../IntegrationTests/geomec_rgi && \
python ./test_rgi.py && \
cd -

#message( test.commands = \"$$test.commands\" )

# debug : the code before it was teh same but with 'debug' hard-coded instead of using $${DESTINATION}

QMAKE_EXTRA_TARGETS += test