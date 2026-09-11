
TEMPLATE = app

GUID = "{333B215F-0ADE-4F95-BE73-4D421DBE2BB2}"

include(../../qmake/config.pri)

CONFIG += console

include(../../qmake/defines.pri)

TARGET = gendiatest

INCLUDEPATH += ../../lib/GeomecModel/SRC
INCLUDEPATH += ../../lib/FemAppModel
INCLUDEPATH += ../../lib/DianaCore/src
INCLUDEPATH += ../../lib/MfcModelFrame
INCLUDEPATH += ../../fw51/GuiFrame/include
INCLUDEPATH += ../../fw51/Geometry/include
INCLUDEPATH += $$DIAINCLUDE
INCLUDEPATH += $(DIASRC)/include
INCLUDEPATH += ../../lib/GeomecModel/BaseModel
INCLUDEPATH += ../../fw51/ModelFrame/include
INCLUDEPATH += ../../lib/GeomecModel/Value
INCLUDEPATH += ../../fw51/OpenGLFrame/include
INCLUDEPATH += $(QTDIR)/include/QtOpenGL
INCLUDEPATH += ../../lib/Quantity/include
INCLUDEPATH += ../../lib/GeomecStringTable/SRC
INCLUDEPATH += ../../lib/NewWellScheme/include

include(../../qmake/destination.pri)
include(../../qmake/flags.pri)

BASE = ../..

include(../../qmake/dependencies.pri)
include(../../qmake/diana.pri)

# Remember, any changes to the LIBS variable have to be duplicated to the
# POST_TARGETDEPS variable (until this has been automated)!

LIBS       += ../../lib/GeomecModel/$${DESTINATION}/libGeomecModel$$LIBRARY_EXTENSION
LIBS       += ../../lib/CommonWellScheme/$${DESTINATION}/libCommonWellScheme$$LIBRARY_EXTENSION
LIBS       += ../../lib/FemAppModel/$${DESTINATION}/libFemAppModel$$LIBRARY_EXTENSION
LIBS       += ../../fw51/ModelFrame/$${DESTINATION}/libModelFrame$$LIBRARY_EXTENSION
LIBS       += ../../lib/Quantity/$${DESTINATION}/libQuantity$$LIBRARY_EXTENSION
LIBS       += ../../lib/ConsoleLibrary/$${DESTINATION}/libConsoleLibrary$$LIBRARY_EXTENSION
LIBS       += ../../lib/GeomecStringTable/$${DESTINATION}/libGeomecStringTable$$LIBRARY_EXTENSION
LIBS       += ../../lib/NewWellScheme/$${DESTINATION}/libNewWellScheme$$LIBRARY_EXTENSION
LIBS       += ../../fw51/OpenGLFrame/$${DESTINATION}/libOpenGLFrame$$LIBRARY_EXTENSION
LIBS       += ../../fw51/FieldValueFile/$${DESTINATION}/libFieldValueFile$$LIBRARY_EXTENSION
LIBS       += ../../lib/WellScheme/$${DESTINATION}/libWellScheme$$LIBRARY_EXTENSION
LIBS       += -L$$DIAPATH/lib -l$$DLLLBCX
LIBS       += -L$$DIAPATH/lib -l$$DLLLBCP
LIBS       += ../../lib/QDianaStartUp/$${DESTINATION}/libQDianaStartUp$$LIBRARY_EXTENSION
LIBS       += ../../lib/DianaCore/$${DESTINATION}/libDianaCore$$LIBRARY_EXTENSION
LIBS       += -L$$DIAPATH/lib -l$$DLLLBFL
LIBS       += ../../fw51/TetMesh/$${DESTINATION}/libTetMesh$$LIBRARY_EXTENSION
LIBS       += ../../fw51/Tensor/$${DESTINATION}/libTensor$$LIBRARY_EXTENSION
LIBS       += ../../lib/DCasingLib/DCasing/$${DESTINATION}/libDCasing$$LIBRARY_EXTENSION
LIBS       += ../../lib/TSMesh/$${DESTINATION}/libTSMesh$$LIBRARY_EXTENSION
LIBS       += ../../lib/RPN/$${DESTINATION}/libRPN$$LIBRARY_EXTENSION
LIBS       += ../../lib/MeshDataCacher/$${DESTINATION}/libMeshDataCacher$$LIBRARY_EXTENSION
LIBS       += ../../fw51/Triangle/$${DESTINATION}/libTriangle$$LIBRARY_EXTENSION
LIBS       += ../../lib/MaterialLibrary/$${DESTINATION}/libMaterialLibrary$$LIBRARY_EXTENSION
LIBS       += ../../lib/MaterialLibraryXML/$${DESTINATION}/libMaterialLibraryXML$$LIBRARY_EXTENSION
LIBS       += ../../lib/CutSurfaceUtilModel/$${DESTINATION}/libCutSurfaceUtilModel$$LIBRARY_EXTENSION
LIBS       += ../../lib/qtiocompressor/$${DESTINATION}/libqtiocompressor$$LIBRARY_EXTENSION
unix:LIBS  += -L$$FWLIB -l$$DLLLBGCCLASSES
win32:LIBS += $$FWLIB/$${DESTINATION}/$$DLLLBGCCLASSES$$LIBRARY_EXTENSION
unix:LIBS  += -L$$FWLIB -l$$DLLLBGCROUTINES
win32:LIBS += $$FWLIB/$${DESTINATION}/$$DLLLBGCROUTINES$$LIBRARY_EXTENSION
LIBS       += -L$$DIAPATH/lib -l$$DLLLBEL
LIBS       += -L$$DIAPATH/lib -l$$DLLLBGS
LIBS       += ../../fw51/Geometry/$${DESTINATION}/libGeometry$$LIBRARY_EXTENSION
LIBS       += -L$$DIAPATH/lib -l$$DLLLBCI
LIBS       += ../../lib/DianaSignals/$${DESTINATION}/libDianaSignals$$LIBRARY_EXTENSION
LIBS       += ../../lib/blosc_filter/$${DESTINATION}/libblosc_filter$$LIBRARY_EXTENSION

CONFIG(debug, debug|release) {
LIBS       += -L../../UserDLLs/$${DESTINATION} -lblosc
}

CONFIG(release, debug|release) {
LIBS       += -L../../UserDLLs -lblosc
}

LIBS += -L ../../lib/tbb/linux/lib/intel64/gcc4.7 -ltbb -ltbbmalloc
unix:LIBS  += -L/usr/lib64 -lglib-2.0
unix:LIBS  += -L/usr/lib65 -lz
unix:LIBS  += ../../lib/hdf5/linux/lib/libhdf5_cpp.a ../../lib/hdf5/linux/lib/libhdf5.a
unix:LIBS  += ../../lib/hdf5/linux/lib/libhdf5_cpp.a ../../lib/hdf5/linux/lib/libhdf5.a
unix:LIBS  += -ldl

QT += xml

POST_TARGETDEPS += ../../lib/GeomecModel/$${DESTINATION}/libGeomecModel$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/CommonWellScheme/$${DESTINATION}/libCommonWellScheme$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/FemAppModel/$${DESTINATION}/libFemAppModel$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/ModelFrame/$${DESTINATION}/libModelFrame$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/Quantity/$${DESTINATION}/libQuantity$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/ConsoleLibrary/$${DESTINATION}/libConsoleLibrary$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/GeomecStringTable/$${DESTINATION}/libGeomecStringTable$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/NewWellScheme/$${DESTINATION}/libNewWellScheme$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/OpenGLFrame/$${DESTINATION}/libOpenGLFrame$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/FieldValueFile/$${DESTINATION}/libFieldValueFile$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/WellScheme/$${DESTINATION}/libWellScheme$$LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBCX$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBCP$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/QDianaStartUp/$${DESTINATION}/libQDianaStartUp$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/DianaCore/$${DESTINATION}/libDianaCore$$LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBFL$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/TetMesh/$${DESTINATION}/libTetMesh$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/Tensor/$${DESTINATION}/libTensor$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/DCasingLib/DCasing/$${DESTINATION}/libDCasing$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/TSMesh/$${DESTINATION}/libTSMesh$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/RPN/$${DESTINATION}/libRPN$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/MeshDataCacher/$${DESTINATION}/libMeshDataCacher$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/Triangle/$${DESTINATION}/libTriangle$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/MaterialLibrary/$${DESTINATION}/libMaterialLibrary$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/MaterialLibraryXML/$${DESTINATION}/libMaterialLibraryXML$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/CutSurfaceUtilModel/$${DESTINATION}/libCutSurfaceUtilModel$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/qtiocompressor/$${DESTINATION}/libqtiocompressor$$LIBRARY_EXTENSION
POST_TARGETDEPS += $$FWLIB/lib$$DLLLBGCCLASSES$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += $$FWLIB/lib$$DLLLBGCROUTINES$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBEL$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBGS$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += ../../fw51/Geometry/$${DESTINATION}/libGeometry$$LIBRARY_EXTENSION
POST_TARGETDEPS += $$DIAPATH/lib/lib$$DLLLBCI$$SHARED_LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/DianaSignals/$${DESTINATION}/libDianaSignals$$LIBRARY_EXTENSION
POST_TARGETDEPS += ../../lib/blosc_filter/$${DESTINATION}/libblosc_filter$$LIBRARY_EXTENSION
CONFIG(debug, debug|release) {
POST_TARGETDEPS += ../../UserDLLs/$${DESTINATION}/libblosc$$SHARED_LIBRARY_EXTENSION
}

CONFIG(release, debug|release) {
POST_TARGETDEPS += ../../UserDLLs/libblosc$$SHARED_LIBRARY_EXTENSION
}

SOURCES += ./main.cpp

# Clean

unix:QMAKE_DISTCLEAN += test test-debug

# Unit test

test.target   =
test.depends  =
test.commands =

unix:test-debug.target   =
unix:test-debug.depends  =
unix:test-debug.commands =

win32:test-debug.target   =
win32:test-debug.depends  =
win32:test-debug.commands =

QMAKE_EXTRA_TARGETS += test test-debug
