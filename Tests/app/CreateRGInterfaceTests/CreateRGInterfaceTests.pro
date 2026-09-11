TEMPLATE = app
CONFIG += console
include(../../qmake/all.pri)

CONFIG(debug, debug|release): 	LIBS += -L$${DLLROOT}/$${DESTINATION} -lRGInterface
CONFIG(release, debug|release): LIBS += -L$${DLLROOT} -lRGInterface

LIBS += $${LBROOT}/RESCUE/$${DESTINATION}/libRESCUE$$LIBRARY_EXTENSION


# Unit test

test.target   = test

# MCR ??
#test.depends  = test_geomec_rgi $${ROOT}/geomec_rgi/release/geomec_rgi rgi

# MCR: TAKE CARE, '\' to concatenate lines add a blank space so command doesn't work
LD_LIBRARY_PATH_=$${DLLROOT}:$(DIAPATH)/release/lib:$${LBROOT}/tbb/linux/lib/intel64/gcc4.7

test.commands = LD_LIBRARY_PATH=$$LD_LIBRARY_PATH_:${LD_LIBRARY_PATH} $(SHELL) ./test_geomec_rgi release && touch test

QMAKE_EXTRA_TARGETS += test