'$IFNDEF HORIZON_INC
'$DEFINE HORIZON_INC


' File name: Horizon.inc
' Project:  Test

SUB NewConstantDepthHorizon(sName As STRING, nDepth As LONG)
	WTreeItemClk("@2", "Horizon\Horizons", VK_RBUTTON)
	WMenuSelect("Create horizon")
	CurrentWindow = WFndWndC("Horizon attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	'Defaultly focus on name
	Play sName
	'Switch to depth
'	WOptionSelect("Constant depth")
	Play "{TAB}{TAB}"
	Play STR(nDepth)
	WButtonClick("OK")
END SUB

SUB DeleteHorizon(sName AS STRING)
	if SelectItem("@2","Horizon\Horizons\" + sName) then
		WTreeItemClk("@2", "Horizon\Horizons\" + sName, VK_RBUTTON)
		WMenuSelect("&Delete")
		'Delete horizon window title
		if WfndWnd("Delete horizon",FW_ACTIVE_ONLY or FW_PART,10) then
			WButtonClick("OK")
		endif
	endif
END SUB

TYPE HORIZON_INFO
	sName As STRING
	bConstantDepth As INTEGER
	dDepth As DOUBLE
	dTop AS DOUBLE
	dBottom AS DOUBLE
	bSlipLayer AS INTEGER
	dFrictionAngle AS DOUBLE
	dCohesion AS DOUBLE
	aSurfaces AS ARRAY OF STRING
END TYPE

FUNCTION GetHorizonInfo(sName As STRING) As HORIZON_INFO
	DIM ret as HORIZON_INFO

	if SelectItem("@2","Horizon\Horizons\" + sName) then
		WTreeItemClk("@2", "Horizon\Horizons\" + sName, VK_RBUTTON)
		WMenuSelect("Attributes")
		Sleep(0.1)
		ret.sName = sName
		ret.dTop = val(StaticText("@5"))
		ret.dBottom = val(StaticText("@6"))
		ret.dDepth = val(EditText("@2"))
		ret.dFrictionAngle = val(EditText("@3"))
		ret.dCohesion = val(EditText("@4"))
		ret.bConstantDepth = WOptionState("@1")
		ret.bSlipLayer = WCheckState("@1")
		WButtonClick ("Cancel")

		Dim aStrings as ARRAY OF STRING
		aStrings = TreeItems("@2","Horizon\Horizons\" + sName)
		ret.aSurfaces = aStrings
	endif

	GetHorizonInfo = ret

END FUNCTION

SUB SetHorizonName(sName AS STRING, sNewName AS STRING)
	if SelectItem("@2","Horizon\Horizons\" + sName) then
		WTreeItemClk("@2", "Horizon\Horizons\" + sName, VK_RBUTTON)
		WMenuSelect ("Attributes")
		WEditSetText ("@1",sNewName)
		WButtonClick ("OK")
		DIM ret as HORIZON_INFO
		ret = GetHorizonInfo(sNewName)
		if not ret.sName = sNewName then
			tnofail "New Horizonname could not be set"
		endif
	endif

END SUB

SUB SetConstantDepthHorizon(sName AS STRING, dDepth as DOUBLE)
	SelectItem("@2","Horizon\Horizons\" + sName)
	WTreeItemClk("@2", "Horizon\Horizons\" + sName, VK_RBUTTON)
	WMenuSelect ("Attributes")
	WOptionSelect ("@1")
	WEditSetText ("@2",str(dDepth))
	WButtonClick ("OK")
	DIM ret as HORIZON_INFO
	ret = GetHorizonInfo(sName)
	if ret.bConstantDepth <> 1 or ret.dDepth <> dDepth then
		tnofail "Horizon could not be made constant depth"
	endif

END SUB

' SetHorizonSlipLayer returns 1 if succeeded, 0 otherwise
FUNCTION SetHorizonSlipLayer(sName AS STRING, dFrictionAngle AS DOUBLE, dCohesion as DOUBLE) AS INTEGER

	Dim retVal as INTEGER
	SelectItem("@2","Horizon\Horizons\" + sName)
	WTreeItemClk("@2", "Horizon\Horizons\" + sName, VK_RBUTTON)
	WMenuSelect ("Attributes")
	WCheckCheck ("@1")
	WEditSetText ("@3",str(dFrictionAngle))
	WEditSetText ("@4",str(dCohesion))
	WButtonClick("OK")
	DIM ret as HORIZON_INFO
	ret = GetHorizonInfo(sName)
	if ret.bSlipLayer <> 1 or ret.dCohesion <> dCohesion or ret.dFrictionAngle <> dFrictionAngle then
		retVal = 0
	else
		retVal = 1
	endif

	SetHorizonSlipLayer = retVal

END FUNCTION

SUB DumpHorizon(nFileId As INTEGER, sName AS STRING)
	DIM horinfo AS HORIZON_INFO
	horInfo = GetHorizonInfo(sName)
	print #nFileId, "Horizon              : " + horInfo.sName
	print #nFileId, "    Top              : " + str(horInfo.dTop)
	print #nFileId, "    Bottom           : " + str(horInfo.dBottom)
	if horInfo.bConstantDepth then
		print #nFileId, "    Depth            : " + str(horInfo.dDepth)
	else
		DIM counter AS LONG
		' print surfaces
		for counter = lbound(horInfo.aSurfaces) to ubound(horInfo.aSurfaces)
			print #nFileId, "    Surface          : " + horInfo.aSurfaces(counter)
		next
	endif
	if horInfo.bSlipLayer then
		print #nFileId, "    FrictionAngle    : " + str(horInfo.dFrictionAngle)
		print #nFileId, "    Cohesion         : " + str(horInfo.dCohesion)
	endif

END SUB


'$ENDIF