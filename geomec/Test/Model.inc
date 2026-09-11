' File name: Model.inc
' Project:  Test

'Model function
FUNCTION TimeSteps() As ARRAY OF STRING
	DIM Size As INTEGER

	'Get the time step tab
	WTreeItemClk("@2", "Horizon", VK_RBUTTON)
	WMenuSelect("Attributes")
	CurrentWindow = WFndWndC("Model attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 449, 337, 382, 323)
	WTabItemClk("@1", "Depletion Stages")

	'Get the list control size
	DIM ret AS ARRAY OF STRING
	REDIM ret(1 TO WViewCount("@1")) As STRING
	DIM item AS LONG
	For item = 1 to WViewCount("@1")
	ret(item) =  ViewItemText("@1", _ord(item), "@1")
	Next item

	'Dismiss dialog
	WButtonClick("OK")

	TimeSteps = ret
END FUNCTION



' to get all the formations in the model
FUNCTION Formations() As ARRAY OF STRING
	Formations = TreeItems("@2", "Horizon\Formations") 
END FUNCTION

' to get all horizons in the model
FUNCTION Horizons() as ARRAY OF STRING
	Horizons = TreeItems("@2","Horizon\Horizons")
END FUNCTION

FUNCTION MeshRegions() AS ARRAY OF STRING
	MeshRegions = TreeItems("@2","Horizon\Mesh")
END FUNCTION

FUNCTION WellPaths() AS ARRAY OF STRING
	WellPaths = TreeItems("@2","Horizon\WellPaths")
END FUNCTION

FUNCTION FormationItemBegin() As Integer
	WTreeItemClk("@2","Horizon\Formations")
	If Not WTreeItemExpanded("","") Then WTreeItemExpand("","")
	FormationItemBegin = WTreeItemIndex("","") + 1
END FUNCTION

FUNCTION FormationItemEnd() As INTEGER
	WTreeItemClk("@2","Horizon\Formations")
	If Not WTreeItemExpanded("","") Then WTreeItemExpand("","")
	DIM startCount AS LONG
	startCount = WTreeItemIndex("","") + 1
	FormationItemEnd = WTreeItemCount("","") + startCount-1
END FUNCTION

FUNCTION FormationName(Index As LONG) As STRING
	Dim pos as String
	pos = _ord(Index)
	If WTreeItemExpanded("@2", pos) Then WTreeItemCollapse("@2", pos)
	FormationName =  TreeItemText("@2", pos)
END FUNCTION

TYPE GLOBAL_INITIAL_STRESS_INFO 
	dVerticalStressGradientDry AS DOUBLE
	dVerticalStressGradientWet AS DOUBLE
	dWaterSurfaceDepth AS DOUBLE
	dWaterColumnStressGradient AS DOUBLE
	dVerticalStressExponent AS DOUBLE
	bDistributed		AS INTEGER
END TYPE

FUNCTION GetGlobalInitialStressInfo() AS GLOBAL_INITIAL_STRESS_INFO
	'Open Dlg Box 
	WTreeItemClk("@2", "Horizon\Initial Stress", VK_RBUTTON)
	WMenuSelect("&Attributes")
	CurrentWindow = WFndWndC("Initial stress attributes...", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	
	'Check values and close
	DIM ret AS GLOBAL_INITIAL_STRESS_INFO 
	ret.dVerticalStressGradientDry = VAL(EditText("@1"))
	ret.dVerticalStressGradientWet = VAL(EditText("@2"))
	ret.dWaterSurfaceDepth = VAL(EditText("@3"))
	ret.dWaterColumnStressGradient = VAL(EditText("@4"))
	ret.dVerticalStressExponent = VAL(EditText("@5"))
	ret.bDistributed = WOptionState("Extrapolated stress values")
	WButtonClick("Cancel")	
	GetGlobalInitialStressInfo = ret

END FUNCTION

SUB SetGlobalInitialStress(VertStrGrdDry As DOUBLE, VertStrGrdWet As DOUBLE, WtrSurfDepth As DOUBLE, WtrClmnStrGrd As DOUBLE, VertStrExp As DOUBLE)
	'Open Dlg Box
	WTreeItemClk("@2", "Horizon\Initial Stress", VK_RBUTTON)
	WMenuSelect("&Attributes")
	CurrentWindow = WFndWndC("Initial stress attributes...", "#32770", FINDWINDOWFLAGS_IF, Timeout)

	'Set Values and close
	WEditSetText("@1", STR(VertStrGrdDry))
	WEditSetText("@2", STR(VertStrGrdWet))
	WEditSetText("@3", STR(WtrSurfDepth))
	WEditSetText("@4", STR(WtrClmnStrGrd))
	WEditSetText("@5", STR(VertStrExp))
	WButtonClick("OK")

	'Open Dlg Box 
	WTreeItemClk("@2", "Horizon\Initial Stress", VK_RBUTTON)
	WMenuSelect("&Attributes")
	CurrentWindow = WFndWndC("Initial stress attributes...", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 426, 383, 429, 230)
	
	'Check values and close
	RecVerifyText("@1", LTRIM(STR(VertStrGrdDry)), 0, V_EDIT)
	RecVerifyText("@2", LTRIM(STR(VertStrGrdWet)), 0, V_EDIT)
	RecVerifyText("@3", LTRIM(STR(WtrSurfDepth)), 0, V_EDIT)
	RecVerifyText("@4", LTRIM(STR(WtrClmnStrGrd)), 0, V_EDIT)
	RecVerifyText("@5", LTRIM(STR(VertStrExp)), 0, V_EDIT)
	WButtonClick("OK")
END SUB

TYPE INITIAL_PORE_PRESSURE_INFO
	dReferencePressure	AS DOUBLE
	dReferenceDepth		AS DOUBLE
	dGradient			AS DOUBLE
	bDistributed		AS INTEGER
END TYPE

FUNCTION GetInitialPorePressureInfo() AS INITIAL_PORE_PRESSURE_INFO
	WTreeItemClk("@2", "Horizon\Initial pore pressure", VK_RBUTTON)
	WMenuSelect("&Attributes")
	DIM ret as INITIAL_PORE_PRESSURE_INFO
	ret.dReferencePressure = val(EditText("@1"))
	ret.dReferenceDepth = val(EditText("@2"))
	ret.dGradient = val(EditText("@3"))
	ret.bDistributed = WOptionState("Extrapolated pressure values")
	WButtonClick("Cancel")	
	GetInitialPorePressureInfo = ret
END FUNCTION

SUB SetInitialPorePressureInfo(dReferencePressure as DOUBLE, dReferenceDepth as DOUBLE, dGradient as DOUBLE, bDistributed as INTEGER)
	' Set values
	WTreeItemClk("@2", "Horizon\Initial pore pressure", VK_RBUTTON)
	WMenuSelect("&Attributes")
	WEditSetText ("@1",str(dReferencePressure))
	WEditSetText ("@2",str(dReferenceDepth))
	WEditSetText ("@3",str(dGradient))
	if bDistributed = 0 then
		WCheckUncheck ("@1")
	else
		WCheckCheck ("@1")
	endif
	WButtonClick ("OK")

	' Check values
	DIM checkVal AS INITIAL_PORE_PRESSURE_INFO
	checkVal = GetInitialPorePressureInfo()
	if checkVal.dReferencePressure <> dReferencePressure OR checkVal.dReferenceDepth <> dReferenceDepth OR checkVal.dGradient <> dGradient OR checkVal.bDistributed <> bDistributed then
		TNOFAIL( "Initial Pore PressureInfo could not be set")
	endif
END SUB


SUB DumpGlobalParam(nFileId AS INTEGER)
	DIM INFO AS GLOBAL_INITIAL_STRESS_INFO
	INFO = GetGlobalInitialStressInfo()
	PRINT #nFileId, "Global initial stresses"
	PRINT #nFileId, ""
	PRINT #nFileId, "Vertical Stress Gradient Dry   : "; STR(INFO.dVerticalStressGradientDry)
	PRINT #nFileId, "Vertical Stress Gradient Wet   : "; STR(INFO.dVerticalStressGradientWet)
	PRINT #nFileId, "Water Surface Depth            : "; STR(INFO.dWaterSurfaceDepth)
	PRINT #nFileId, "Water Column Stress Gradient   : "; STR(INFO.dWaterColumnStressGradient)
	PRINT #nFileId, "Vertical Stress Exponent       : "; STR(INFO.dVerticalStressExponent)
	PRINT #nFileId, "Distributed outside convex hull: "; STR(INFO.bDistributed)
	PRINT #nFileId, ""
	PRINT #nFileId, ""

	DIM pressinfo AS INITIAL_PORE_PRESSURE_INFO
	pressinfo = GetInitialPorePressureInfo()
	PRINT #nFileId, "Initial Pore Pressure"
	PRINT #nFileId, ""
	PRINT #nFileId, "Reference Pressure             : "; STR(pressinfo.dReferencePressure)
	PRINT #nFileId, "Reference Depth                : "; STR(pressinfo.dReferenceDepth)
	PRINT #nFileId, "Pressure Gradient              : "; STR(pressinfo.dGradient)
	PRINT #nFileId, "Distributed outside convex hull: "; STR(pressinfo.bDistributed)
	PRINT #nFileId, ""
	PRINT #nFileId, ""

END SUB
			


SUB DumpFormations(nFileId AS INTEGER)
	DIM arFormation AS ARRAY OF STRING
	arFormation = Formations()
	PRINT #nFileId, "***	Formations	***"
	DIM x AS LONG
	for x = LBOUND(arFormation) to UBOUND(arFormation)
		PRINT #nFileId, ""
		DumpFormation(nFileId, arFormation(x))
	next x
END SUB

SUB DumpHorizons(nFileId AS INTEGER)
	DIM arHorizon AS ARRAY OF STRING
	arHorizon = Horizons()
	PRINT #nFileId, "***	Horizons	***"
	DIM x AS LONG
	for x = LBOUND(arHorizon) to UBOUND(arHorizon)
		PRINT #nFileId, ""
		DumpHorizon(nFileId, arHorizon(x))
	next x
END SUB

SUB DumpMeshRegions(nFileId AS INTEGER)
	DIM arMeshRegion AS ARRAY OF STRING
	arMeshRegion = MeshRegions()
	PRINT #nFileId, "***	MeshRegions	***"
	DIM x AS LONG
	for x = LBOUND(arMeshRegion) to UBOUND(arMeshRegion)
		PRINT #nFileId, ""
		DumpMeshRegion(nFileId, arMeshRegion(x))
	next x
END SUB

SUB DumpWellPaths(nFileId AS INTEGER)
	DIM arWellPaths AS ARRAY OF STRING
	arWellPaths = WellPaths()
	PRINT #nFileId, "***	WellPaths	***"
	DIM x AS LONG
	if not isempty(arWellPaths) then
		for x = LBOUND(arWellPaths) to UBOUND(arWellPaths)
			PRINT #nFileId, ""
			DumpWellPath(nFileId, arWellPaths(x))
		next x
	endif
END SUB


SUB DumpModel(sPath AS STRING)
	DIM nFileId AS INTEGER
	nFileId = FREEFILE
	OPEN sPath FOR OUTPUT AS #nFileId
	PRINT #nFileId, "***	Geomec Model Dump	***"
	PRINT #nFileId, ""
	'make sure tree is active
	Play "{Click 75, 75, Left}"
	DumpGlobalParam(nFileId)
	DumpTimeStep(nFileId)
	DumpHorizons(nFileId)
	DumpFormations(nFileId)
	DumpMeshRegions(nFileId)
	DumpWellPaths(nFileId)
	
	CLOSE #nFileId 	
END SUB


	
	