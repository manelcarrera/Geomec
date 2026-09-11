DocDirs =	DocTetMesh			DocTriangle		DocDianaCore		DocQuantity			\
			DocFemApp			DocXMLLib		Doc2DWellViewLib	DocDCasingLib		\
			DocDoubleEditLib	DocGenUtils		DocGmDatabase		DocMeshDataCacher	\
			DocQDianaStartUp	DocRPN			DocTextfile_Parser	DocTSMesh			\
			DocWellMatLib		DocWellScene3D	DocWellScheme		DocWellSchemeUtils	\
			DocMechFunctionsLib 

All: TetMesh_lib Triangle_lib 

Documentation:	$(DocDirs)

TetMesh_lib:
	-( cd TetMesh; $(MAKE) $(MFLAGS) )

Triangle_lib:
	-( cd Triangle; $(MAKE) $(MFLAGS) )

DocTetMesh:
	-( cd TetMesh; $(MAKE) $(MFLAGS) Documentation )

DocTriangle:
	-( cd Triangle; $(MAKE) $(MFLAGS) Documentation )

DocDianaCore:
	-( cd DianaCore; $(MAKE) $(MFLAGS) Documentation )

DocQuantity:
	-( cd Quantity; $(MAKE) $(MFLAGS) Documentation )

DocFemApp:
	-( cd FemApp; $(MAKE) $(MFLAGS) Documentation )

DocXMLLib:
	-( cd XMLLib; $(MAKE) $(MFLAGS) Documentation )

Doc2DWellViewLib:
	-( cd 2DWellViewLib; $(MAKE) $(MFLAGS) Documentation )

DocDCasingLib:
	-( cd DCasingLib; $(MAKE) $(MFLAGS) Documentation )

DocDoubleEditLib:
	-( cd DoubleEditLib; $(MAKE) $(MFLAGS) Documentation )

DocGenUtils:
	-( cd GenUtils; $(MAKE) $(MFLAGS) Documentation )

DocGmDatabase:
	-( cd GmDatabase; $(MAKE) $(MFLAGS) Documentation )

DocMeshDataCacher:
	-( cd MeshDataCacher; $(MAKE) $(MFLAGS) Documentation )

DocQDianaStartUp:
	-( cd QDianaStartUp; $(MAKE) $(MFLAGS) Documentation )

DocRPN:
	-( cd RPN; $(MAKE) $(MFLAGS) Documentation )

DocTextfile_Parser:
	-( cd Textfile_Parser; $(MAKE) $(MFLAGS) Documentation )

DocTSMesh:
	-( cd TSMesh; $(MAKE) $(MFLAGS) Documentation )

DocWellMatLib:
	-( cd WellMatLib; $(MAKE) $(MFLAGS) Documentation )

DocWellScene3D:
	-( cd WellScene3D; $(MAKE) $(MFLAGS) Documentation )

DocWellScheme:
	-( cd WellScheme; $(MAKE) $(MFLAGS) Documentation )

DocWellSchemeUtils:
	-( cd WellSchemeUtils; $(MAKE) $(MFLAGS) Documentation )

DocMechFunctionsLib:
	-( cd MechFunctionsLib; $(MAKE) $(MFLAGS) Documentation )
