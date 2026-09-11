
TEMPLATE = lib

GUID = "{56A0ADA9-5EE4-4D4F-9CC9-B3CF762E163D}"

include(../../qmake/config.pri)

CONFIG += staticlib

include(../../qmake/defines.pri)

FWROOT = ../../fw51
GMROOT = ../../geomec3
LBROOT = ../../lib

INCLUDEPATH += $(LBROOT)/gmock-1.7.0/gtest/include;
INCLUDEPATH += .
INCLUDEPATH += $(FWROOT)/FieldValueFile/include
INCLUDEPATH += $(FWROOT)/Geometry/Base
INCLUDEPATH += $(FWROOT)/Geometry/Element
INCLUDEPATH += $(FWROOT)/Geometry/include
INCLUDEPATH += $(FWROOT)/GuiFrame/include
INCLUDEPATH += $(FWROOT)/ModelFrame/include
INCLUDEPATH += $(FWROOT)/OpenGLFrame/Base
INCLUDEPATH += $(FWROOT)/OpenGLFrame/include
INCLUDEPATH += $(FWROOT)/Tensor/include
INCLUDEPATH += $(FWROOT)/TetMesh/include
INCLUDEPATH += $(FWROOT)/TetMesh/tet_src
INCLUDEPATH += $(FWROOT)/Triangle/Base
INCLUDEPATH += $(FWROOT)/Triangle/Triangle
INCLUDEPATH += $(FWROOT)/Triangle/include
INCLUDEPATH += $(GMROOT)/SRC
INCLUDEPATH += $(GMROOT)/2D/src
INCLUDEPATH += $(GMROOT)/BaseModel/SRC
INCLUDEPATH += $(GMROOT)/DStaborLink/src
INCLUDEPATH += $(GMROOT)/Excel/src
INCLUDEPATH += $(GMROOT)/FlexLM/License
INCLUDEPATH += $(GMROOT)/FlexLM/include
INCLUDEPATH += $(GMROOT)/FlexLM/lib64
INCLUDEPATH += $(GMROOT)/GUI
INCLUDEPATH += $(GMROOT)/GeomecModel/SRC
INCLUDEPATH += $(GMROOT)/HexaModel/SRC
INCLUDEPATH += $(GMROOT)/Material/GUI
INCLUDEPATH += $(GMROOT)/Material/src
INCLUDEPATH += $(GMROOT)/ShellExecute
INCLUDEPATH += $(GMROOT)/SurfaceThinOut
INCLUDEPATH += $(GMROOT)/TcxMsgBox
INCLUDEPATH += $(GMROOT)/TetraModel/Eclipse/SRC
INCLUDEPATH += $(GMROOT)/TetraModel/GoCad
INCLUDEPATH += $(GMROOT)/TetraModel/SRC
INCLUDEPATH += $(GMROOT)/Value/SRC
INCLUDEPATH += $(GMROOT)/View/SRC
INCLUDEPATH += $(GMROOT)/WellZoomInModel/src
INCLUDEPATH += $(GMROOT)/Zoomin
INCLUDEPATH += $(GMROOT)/Geology/SRC
INCLUDEPATH += $(GMROOT)/Geology/AttriDlg/SRC
INCLUDEPATH += $(LBROOT)/2DWellViewLib
INCLUDEPATH += $(LBROOT)/ConsoleLibrary
INCLUDEPATH += $(LBROOT)/CutSurfaceUtil
INCLUDEPATH += $(LBROOT)/CutSurfaceUtilModel
INCLUDEPATH += $(LBROOT)/DCasingLib/DCasing
INCLUDEPATH += $(LBROOT)/DianaCore
INCLUDEPATH += $(LBROOT)/DianaCore/Loads/src
INCLUDEPATH += $(LBROOT)/DianaCore/src
INCLUDEPATH += $(LBROOT)/DianaSignals
INCLUDEPATH += $(LBROOT)/DoubleEditLib
INCLUDEPATH += $(LBROOT)/FemAppGUI/SRC
INCLUDEPATH += $(LBROOT)/FemAppModel
INCLUDEPATH += $(LBROOT)/GeomecModel/2D
INCLUDEPATH += $(LBROOT)/GeomecModel/BaseModel
INCLUDEPATH += $(LBROOT)/GeomecModel/Geology
INCLUDEPATH += $(LBROOT)/GeomecModel/HexaModel
INCLUDEPATH += $(LBROOT)/GeomecModel/Material
INCLUDEPATH += $(LBROOT)/GeomecModel/RGInterface
INCLUDEPATH += $(LBROOT)/GeomecModel/SRC
INCLUDEPATH += $(LBROOT)/GeomecModel/TetraModel
INCLUDEPATH += $(LBROOT)/GeomecModel/TetraModel/Eclipse
INCLUDEPATH += $(LBROOT)/GeomecModel/TetraModel/GoCad
INCLUDEPATH += $(LBROOT)/GeomecModel/Value
INCLUDEPATH += $(LBROOT)/GeomecModel/View
INCLUDEPATH += $(LBROOT)/GeomecModel/WellZoomInModel
INCLUDEPATH += $(LBROOT)/GeomecModel/Zoomin
INCLUDEPATH += $(LBROOT)/GeomecStringTable/SRC
INCLUDEPATH += $(LBROOT)/GlobalMessage
INCLUDEPATH += $(LBROOT)/GmDatabase
INCLUDEPATH += $(LBROOT)/GmDatabase/SRC
INCLUDEPATH += $(LBROOT)/GmDatabase/include
INCLUDEPATH += $(LBROOT)/MaterialLibrary/include
INCLUDEPATH += $(LBROOT)/MaterialLibraryXML/include
INCLUDEPATH += $(LBROOT)/MeshDataCacher
INCLUDEPATH += $(LBROOT)/MfcGuiFrame/include
INCLUDEPATH += $(LBROOT)/MfcGuiFrame/src
INCLUDEPATH += $(LBROOT)/MfcModelFrame
INCLUDEPATH += $(LBROOT)/NewWellScheme/include
INCLUDEPATH += $(LBROOT)/QDianaStartUp
INCLUDEPATH += $(LBROOT)/Quantity
INCLUDEPATH += $(LBROOT)/Quantity/include
INCLUDEPATH += $(LBROOT)/RESCUE
INCLUDEPATH += $(LBROOT)/RGInterface/SRC
INCLUDEPATH += $(LBROOT)/RPN
INCLUDEPATH += $(LBROOT)/RPN/SRC
INCLUDEPATH += $(LBROOT)/TSMesh
INCLUDEPATH += $(LBROOT)/TSMesh/Gts
INCLUDEPATH += $(LBROOT)/TSMesh/glib
INCLUDEPATH += $(LBROOT)/TSMesh/glib/gmodule
INCLUDEPATH += $(LBROOT)/TSMesh/glib/gobject
INCLUDEPATH += $(LBROOT)/TSMesh/src
INCLUDEPATH += $(LBROOT)/WellScene3D
INCLUDEPATH += $(LBROOT)/WellScheme
INCLUDEPATH += $(LBROOT)/WellSchemeUtils
INCLUDEPATH += $(LBROOT)/WindowsRegistry
INCLUDEPATH += $(LBROOT)/XMLLib
INCLUDEPATH += $(LBROOT)/qtiocompressor/src
INCLUDEPATH += $(LBROOT)/qtwinmigrate/src
INCLUDEPATH += $(LBROOT)/zlib-1.2.6
INCLUDEPATH += $(DIASRCINCLUDE)
INCLUDEPATH += $(DIAOBJINCLUDE)
INCLUDEPATH += $(DIAOBJ)/fl44
INCLUDEPATH += $(QTDIR)/include
INCLUDEPATH += $(QTDIR)/include/QtCore
INCLUDEPATH += $(QTDIR)/include/QtGui
INCLUDEPATH += $(QTDIR)/include/QtOpenGL
INCLUDEPATH += $(QTDIR)/include/QtXml
INCLUDEPATH += $(QWTDIR)/src

include(../../qmake/destination.pri)
include(../../qmake/flags.pri)

BASE = ../..

include(../../qmake/dependencies.pri)

SOURCES += ./YOURFILE.cpp

HEADERS += ./YOURFILE.h

# Unit test

test.depends  =
test.commands =

test-debug.depends  =
test-debug.commands =

QMAKE_EXTRA_TARGETS += test test-debug
