
TEMPLATE = subdirs

include(qmake/config.pri)
include(qmake/defines.pri)

# addSubdirs(subdirs,deps): Adds directories to the project that depend on
# other directories

defineTest( addSubdirs ) {
	for(subdirs, 1) {
		entries = $$files($$subdirs)
		for(entry, entries) {
			name = $$replace(entry, [/\\\\], _)
			name = $$replace(name, - , _)
			SUBDIRS += $$name
			eval ($${name}.subdir = $$entry)
			for(dep, 2): {
				tempval = $$replace(dep, [/\\\\], _)
				eval ($${name}.depends += $$replace(tempval, - , _) )
			}
			export ($${name}.subdir)
			export ($${name}.depends)
		}
	}
	export (SUBDIRS)
}

# the sub-directories

# libraries

addSubdirs (fw51/FieldValueFile)
addSubdirs (fw51/Geometry)
addSubdirs (fw51/GuiFrame) # never used
addSubdirs (fw51/ModelFrame)
addSubdirs (fw51/OpenGLFrame)
addSubdirs (fw51/Tensor)
addSubdirs (fw51/TetMesh)
addSubdirs (fw51/Triangle)

addSubdirs (lib/blosc_filter)
addSubdirs (lib/blosc)
addSubdirs (lib/ConsoleLibrary)
addSubdirs (lib/CutSurfaceUtilModel)
addSubdirs (lib/DCasing)
addSubdirs (lib/DianaCore)
addSubdirs (lib/DianaSignals)
addSubdirs (lib/FemAppModel)
addSubdirs (lib/GeomecModel)
addSubdirs (lib/GeomecStringTable)
addSubdirs (lib/GMSkuaConnector)
addSubdirs (lib/Util)
addSubdirs (lib/QUtil)

addSubdirs (lib/MaterialLibrary)
addSubdirs (lib/MaterialLibraryXML)
addSubdirs (lib/MeshDataCacher)
addSubdirs (lib/NewWellScheme)
addSubdirs (lib/DianaStartup)
addSubdirs (lib/qtiocompressor)
addSubdirs (lib/Quantity)
addSubdirs (lib/RESCUE)
addSubdirs (lib/RGInterface)
addSubdirs (lib/RPN)
addSubdirs (lib/TSMesh)
addSubdirs (lib/WellScene3D) # never used
addSubdirs (lib/WellScheme)
addSubdirs (lib/geomec_rgi_library)
addSubdirs (lib/geomec_cora_library)
addSubdirs (lib/CommonWellScheme)
addSubdirs (lib/DianaGlue)
#addSubdirs (qmake) # MCR

SUBDIRS     += gtest
gtest.subdir = lib/gmock-1.7.0/gtest

addSubdirs (Tests/SanityChecks)
addSubdirs (Tests/Fw51)
addSubdirs (Tests/Materials)
addSubdirs (Tests/ModelOperations)
addSubdirs (Tests/TestLib)

addSubdirs (Prototypes/ModularGeomec/ModGMBus)

# executables

kk {
addSubdirs (CreateRGInterfaceTests, \
	\
	lib/RGInterface \
	lib/RESCUE \
	geomec_rgi)

addSubdirs (geomec_launcher, \
	\
	lib/Util \
	lib/QUtil \
	Prototypes/ModularGeomec/ModGMBus )

addSubdirs (createfilos_app, \
	\
	lib/QUtil \
	Prototypes/ModularGeomec/ModGMBus )

addSubdirs (diana_app, \
	\
	lib/Util \
	lib/QUtil \
	lib/DianaStartup \
	lib/DianaCore \
	lib/DianaSignals \
	fw51/Tensor \
	fw51/Geometry \
	Prototypes/ModularGeomec/ModGMBus )
}

addSubdirs (gm_diana_app, \
	\
	lib/Util \
	lib/QUtil \
	lib/DianaStartup \
	lib/DianaCore )


kk {
	addSubdirs (gendiatest, \
		\
		lib/GeomecModel \
		lib/FemAppModel \
		fw51/ModelFrame \
		lib/Quantity \
		lib/ConsoleLibrary \
		lib/GeomecStringTable \
		lib/NewWellScheme \
		fw51/OpenGLFrame \
		fw51/FieldValueFile \
		lib/WellScheme \
		lib/DianaStartup \
		lib/DianaCore \
		fw51/TetMesh \
		fw51/Tensor \
		lib/DCasing \
		lib/TSMesh lib/RPN \
		lib/MeshDataCacher \
		fw51/Triangle \
		lib/MaterialLibrary \
		lib/MaterialLibraryXML \
		lib/CutSurfaceUtilModel \
		lib/qtiocompressor \
		fw51/Geometry \
		lib/DianaSignals \
		lib/CommonWellScheme)

addSubdirs (GmDianaMain, \
	\
	lib/Util \
	lib/QUtil \
	Tests/DianaStandAlone/DianaGlue)

}

addSubdirs (geomec_shell, \
	\
	fw51/FieldValueFile \
	fw51/ModelFrame \
	fw51/TetMesh lib/TSMesh \
	fw51/OpenGLFrame \
	fw51/Tensor \
	fw51/Triangle \
	fw51/Geometry \
	lib/geomec_rgi_library \
	lib/geomec_cora_library \
	lib/GeomecStringTable \
	lib/GeomecModel \
	lib/Quantity \
	lib/ConsoleLibrary \
	lib/MaterialLibrary \
	lib/DianaCore \
	lib/FemAppModel \
	lib/MaterialLibraryXML \
	lib/CutSurfaceUtilModel \
	lib/NewWellScheme \
	lib/WellScheme \
	lib/DianaStartup \
	lib/DCasing \
	lib/RPN \
	lib/MeshDataCacher \
	lib/qtiocompressor \
	lib/DianaSignals \
	lib/CommonWellScheme \
	lib/Util \
	lib/QUtil \
	lib/DianaGlue \
	Prototypes/ModularGeomec/ModGMBus )

addSubdirs (geomec_cora_wrapper)

kk {
addSubdirs (geomec_cora, \
	\
	fw51/FieldValueFile \
	fw51/ModelFrame \
	fw51/TetMesh lib/TSMesh \
	fw51/OpenGLFrame \
	fw51/Tensor \
	fw51/Triangle \
	fw51/Geometry \
	lib/geomec_cora_library \
	lib/GeomecStringTable \
	lib/GeomecModel \
	lib/Quantity \
	lib/ConsoleLibrary \
	lib/MaterialLibrary \
	lib/DianaCore \
	lib/FemAppModel \
	lib/MaterialLibraryXML \
	lib/CutSurfaceUtilModel \
	lib/NewWellScheme \
	lib/WellScheme \
	lib/DianaStartup \
	lib/DCasing \
	lib/RPN \
	lib/MeshDataCacher \
	lib/qtiocompressor \
	lib/DianaSignals \
	lib/CommonWellScheme \
	lib/Util \
	lib/QUtil \
	lib/DianaGlue \
	Prototypes/ModularGeomec/ModGMBus )

addSubdirs (geomec_rgi, \
	\
	fw51/ModelFrame \
	fw51/TetMesh \
	fw51/FieldValueFile \
	fw51/OpenGLFrame \
	fw51/Tensor \
	fw51/Triangle \
	fw51/Geometry \
	lib/GeomecModel \
	lib/geomec_rgi_library \
	lib/FemAppModel \
	lib/RPN \
	lib/GeomecStringTable \
	lib/DianaCore \
	lib/ConsoleLibrary \
	lib/Quantity \
	lib/NewWellScheme \
	lib/WellScheme \
	lib/MaterialLibrary \
	lib/DianaStartup \
	lib/DCasing \
	lib/MeshDataCacher \
	lib/TSMesh \
	lib/MaterialLibraryXML \
	lib/CutSurfaceUtilModel \
	lib/DianaSignals \
	lib/qtiocompressor \
	lib/CommonWellScheme \
	lib/Util \
	lib/QUtil \
	lib/DianaGlue \
	Prototypes/ModularGeomec/ModGMBus )

addSubdirs (geomec_rgi_lock, \
	\
	lib/RGInterface \
	lib/RESCUE)

}

addSubdirs (Tests/Tests, \
	\
	fw51/ModelFrame \
	fw51/FieldValueFile \
	fw51/OpenGLFrame \
	fw51/Tensor \
	fw51/TetMesh \
	fw51/Geometry \
	fw51/Triangle \
	lib/geomec_cora_library \
	lib/GeomecModel \
	lib/FemAppModel \
	lib/Quantity \
	lib/ConsoleLibrary \
	lib/GeomecStringTable \
	lib/NewWellScheme \
	lib/DianaStartup \
	lib/DianaCore \
	lib/WellScheme \
	lib/DCasing \
	lib/RPN \
	lib/MeshDataCacher \
	lib/TSMesh \
	lib/MaterialLibrary \
	lib/CutSurfaceUtilModel \
	lib/MaterialLibraryXML \
	lib/DianaSignals \
	lib/qtiocompressor \
	lib/CommonWellScheme\
	gtest \
	Tests/SanityChecks \
	Tests/Fw51 \
	Tests/Materials \
	Tests/ModelOperations \
	Tests/TestLib \
	lib/Util \
	lib/QUtil \
	lib/DianaGlue \
	Prototypes/ModularGeomec/ModGMBus )

# Unit test
test.depends = release
test.target  = test
test.CONFIG  = recursive

installer.depends = test
installer.target = installer
installer.commands = cd test/geomec_rgi && python create_installation.py && cd -

installer-no-test.depends = release
installer-no-test.target = installer-no-test
#installer-no-test.commands = $$installer.commands

QMAKE_EXTRA_TARGETS += test
QMAKE_EXTRA_TARGETS += installer
QMAKE_EXTRA_TARGETS += installer-no-test