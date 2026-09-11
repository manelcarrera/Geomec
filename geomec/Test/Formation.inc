' File name: Formation.inc
' Project:  Test
'$ifndef FORMATION_INC
'$define FORMATION_INC

TYPE FORMATION_INFO
	sName				AS STRING
	sUpperHorizon		AS STRING
	sLowerHorizon		AS STRING
	sMaterial			AS STRING
	aPressures			AS ARRAY OF STRING	' for every depletion stage
	iNumberOfElements	AS INTEGER
	iMinLayerThickness	AS INTEGER
END TYPE

FUNCTION GetFormationInfo(sName AS STRING) AS FORMATION_INFO

	DIM ret as FORMATION_INFO

	if SelectItem("@2","Horizon\Formations\" + sName) then
		WTreeItemClk("@2", "Horizon\Formations\" + sName, VK_RBUTTON)
		WMenuSelect("Attributes")
		ret.sName = EditText("@1")
		ret.iMinLayerThickness = val(EditText("@2"))
		ret.iNumberOfElements = val(EditText("@3"))
		WButtonClick "Cancel"

		Dim aStrings as ARRAY OF STRING
		aStrings = TreeItems("@2","Horizon\Formations\" + sName)
		' we are interested in the third one (material)
		ret.sMaterial = aStrings(3)
		aStrings = TreeItems("@2","Horizon\Formations\" + sName + "\Horizons")
		ret.sUpperHorizon = aStrings(1)
		ret.sLowerHorizon = aStrings(2)
		aStrings = TreeItems("@2","Horizon\Formations\" + sName + "\Pressure")
		ret.aPressures = aStrings
	endif

	GetFormationInfo = ret
END FUNCTION

SUB SetFormationInfo(sName as STRING, tInfo as FORMATION_INFO)
	WTreeItemClk("@2","Horizon\Formations\" + sName, VK_RBUTTON)
	WMenuSelect("Attributes")
	WEditSetText("@1",tInfo.sName)
	WEditSetText("@2",str(tInfo.iMinLayerThickness))
	WEditSetText("@3",str(tInfo.iNumberOfElements))
	WButtonClick "OK"

	DIM retVal as FORMATION_INFO
	retVal = GetFormationInfo(tInfo.sName)
	if not retVal.sName = tInfo.sName then
		tnofail "Formation attributes dialog did not accept name"
	endif
	if not retVal.iMinLayerThickness = tInfo.iMinLayerThickness then
		tnofail "Formation attributes dialog did not accept minlayerthickness"
	endif
	if not retVal.iNumberOfElements = tInfo.iNumberOfElements then
		tnofail "Formation attributes dialog did not accept numberofelements"
	endif
END SUB

TYPE PRESSURE_INFO
	nType AS INTEGER
	dPressure AS DOUBLE
	dDepth AS DOUBLE
	dGradient AS DOUBLE
	dLowerGradient AS DOUBLE
	bConvexHull AS INTEGER
END TYPE

FUNCTION GetPressureInfo(sFormation AS STRING, sTimeStep AS STRING) AS PRESSURE_INFO
	DIM ret AS PRESSURE_INFO
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")

	' Is option same as prev enabled?
	IF WOptionExists("Global pore pressure") THEN
		IF WOptionEnabled("Global pore pressure") THEN
			ret.nType = 1
		END IF
	ELSE
		IF WOptionEnabled("Same as previous depletion stage") THEN
			ret.nType = 1
		END IF
	END IF

	' Constant
	IF	WOptionEnabled("&Single value") THEN
		ret.nType = 2
		ret.dPressure = VAL(EditText("@1"))
		ret.dDepth = VAL(EditText("@2"))
		ret.dGradient = VAL(EditText("@3"))
	END IF

	' GWC / OWC
	IF WOptionEnabled ("&GWC / OWC") THEN
		ret.nType = 3
		ret.dPressure = VAL(EditText("@4"))
		ret.dDepth = VAL(EditText("@5"))
		ret.dGradient = VAL(EditText("@6"))
		ret.dLowerGradient = VAL(EditText("@7"))
	END IF

	ret.bConvexHull = WOptionEnabled("Use distributed stress outside convex hull")

	WButtonClick("OK")
END FUNCTION

SUB SetPressureSameAsPrevious(sFormation AS STRING, sTimeStep AS STRING)
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")
	IF WOptionExists("Global pore pressure") THEN
		WOptionSelect("Global pore pressure")
	ELSE
		WOptionSelect("Same as previous depletion stage")
	END IF
	WButtonClick("OK")		
END SUB

SUB SetPressureConstant(sFormation AS STRING, sTimeStep AS STRING, dPressure AS DOUBLE, dDepth AS DOUBLE, dGradient AS DOUBLE)
	'Set constant pressure
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")
	Sleep(0.1)
	WOptionSelect("&Single value")
	WEditSetText("#1311", STR(dPressure))
	WEditSetText("#1312", STR(dDepth))
	WEditSetText("#1313", STR(dGradient))
	WButtonClick("OK")		
	
	'Verify
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")	

	RecVerifyEnabled("&Single value", NULL, V_OPTION, TRUE)
	RecVerifyText("#1311", LTRIM(STR(dPressure)), 0, V_EDIT)
	RecVerifyText("#1312", LTRIM(STR(dDepth)), 0, V_EDIT)
	RecVerifyText("#1313", LTRIM(STR(dGradient)), 0, V_EDIT)
	WButtonClick("OK")

END SUB

SUB SetPressureGWC(sFormation AS STRING, sTimeStep AS STRING, dPressure AS DOUBLE, dDepth AS DOUBLE, dGradient AS DOUBLE)
	'Set constant pressure
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")
	WOptionSelect("&GWC / OWC")
	WEditSetText("@4", STR(dPressure))
	WEditSetText("@5", STR(dDepth))
	WEditSetText("@6", STR(dGradient))
	WEditSetText("@7", STR(dLowerGradient))
	WButtonClick("OK")		
	
	'Verify
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\Pressure\" + sTimeStep, VK_RBUTTON)
	WMenuSelect("&Attributes")	

	RecVerifyEnabled("&GWC / OWC", NULL, V_OPTION, TRUE)
	RecVerifyText("@1", LTRIM(STR(dPressure)), 0, V_EDIT)
	RecVerifyText("@2", LTRIM(STR(dDepth)), 0, V_EDIT)
	RecVerifyText("@3", LTRIM(STR(dGradient)), 0, V_EDIT)
	WButtonClick("OK")

END SUB

SUB DumpFormation(nFileId AS INTEGER, sFormation AS STRING)
	DIM tInfo as FORMATION_INFO
	tInfo = GetFormationInfo(sFormation)
	PRINT #nFileId, "Name                   : "; tInfo.sName
	PRINT #nFileId, "Min Layer Thickness    : "; tInfo.iMinLayerThickness 
	PRINT #nFileId, "Nr of elements         : "; tInfo.iNumberOfElements 
	PRINT #nFileId, "" 
	PRINT #nFileId, "Horizons:"
	
	' Retrieve horizons
	DIM arHorizon AS ARRAY OF STRING
	arHorizon = TreeItems("@2", "Horizon\Formations\" + sFormation + "\Horizons")
	for x = LBOUND(arHorizon) to UBOUND(arHorizon)
	PRINT #nFileId, arHorizon(x)
	next x

	' Retrieve pressure

END SUB
	
'$endif