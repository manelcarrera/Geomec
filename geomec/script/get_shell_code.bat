svn co svn://srv-nfs.diana.local/geomec/trunk/geomec3
del /q geomec3\keyfile
if not exist lib mkdir lib
cd lib
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/2DWellViewLib
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/ConsoleLibrary
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/CreateRGInterfaceTests
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/CutSurfaceUtil
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/CutSurfaceUtilModel
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/DCasingLib
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/DianaCore
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/DianaSignals
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/DoubleEditLib
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/FemAppGUI
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/FemAppModel
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/GeomecModel
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/GeomecStringTable
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/GlobalMessage
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/GmDatabase
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/MaterialLibrary
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/MaterialLibraryXML
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/MeshDataCacher
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/MfcGuiFrame
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/MfcModelFrame
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/NewWellScheme
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/QDianaStartUp
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/qtiocompressor
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/qtwinmigrate
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/Quantity
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/RESCUE
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/RGInterface
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/RPN
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestDelegate
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestFileCopy
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestFileRegistry
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestGeomecArchive
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestGeomecStringTable
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestGlobalMessage
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestQDateTimeCOleDateTime
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestQStringCString
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestRegistry
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TestSTL
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/TSMesh
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/WellScene3D
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/WellScheme
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/WellSchemeUtils
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/WindowsRegistry
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/XMLLib
svn co svn://srv-nfs.diana.local/geomec/trunk/lib/zlib-1.2.6
cd ..
del geomec_shell_code.zip
zip -r -9 geomec_shell_code.zip geomec3 lib
