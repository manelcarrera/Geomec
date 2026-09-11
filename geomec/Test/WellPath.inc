' File name: WellPath.inc
' Project:  Test
'$ifndef WELLPATH_INC
'$define WELLPATH_INC

TYPE WELLPATH_INFO
	sName				AS STRING
	dNorthing			AS DOUBLE
	dEasting			AS DOUBLE
	dDerrickElevation	AS DOUBLE
END TYPE

SUB WellPathStartUp
	StartApp
	LoadModel(curdir$,"test.gmp")
	Sleep(5)
END SUB

FUNCTION GetWellPathInfo(sName AS STRING) AS WELLPATH_INFO
	DIM ret as WELLPATH_INFO

	if SelectItem("@2","Horizon\WellPaths\" + sName) then
		WTreeItemClk("@2", "Horizon\WellPaths\" + sName, VK_RBUTTON)
		WMenuSelect("Attributes")
		ret.sName = EditText("@1")
		ret.dNorthing = val(EditText("@2"))
		ret.dEasting = val(EditText("@3"))
		ret.dDerrickElevation = val(EditText("@4"))
		WButtonClick("Cancel")
	endif

	GetWellPathInfo = ret

END FUNCTION

FUNCTION SetWellPathInfo(sName as STRING, info AS WELLPATH_INFO, bAllowFail = 0 AS INTEGER) AS STRING
	if SelectItem("@2","Horizon\WellPaths\" + sName) then
		WTreeItemClk("@2", "Horizon\WellPaths\" + sName, VK_RBUTTON)
		WMenuSelect("Attributes")
		WEditSetText("@1",info.sName)
		WEditSetText("@2",str(info.dNorthing))
		WEditSetText("@3",str(info.dEasting))
		WEditSetText("@4",str(info.dDerrickElevation))
		WButtonClick("OK")
		DIM retInfo as WELLPATH_INFO
		DIM failString as STRING
		retInfo = GetWellPathInfo(info.sName)
		if retInfo.dNorthing <> info.dNorthing then
			failString = "Failed to set northing"
		endif
		if retInfo.dEasting <> info.dEasting then
			failString = "Failed to set easting"
		endif
		if retInfo.dDerrickElevation <> info.dDerrickElevation then
			failString = "Failed to set derrickelevation"
		endif
		if bAllowFail = 0 and len(failString) > 0 then
			tnofail failString
		endif
	endif
	SetWellPathInfo = failString
END SUB

SUB WellPathMouseClick(dNorthing AS DOUBLE, dEasting AS DOUBLE)
	Dim playString AS STRING
	playString = "{ CLICK " + str(int((dEasting*3.6)+636)) + "," + str(int(720 - (dNorthing*3.6))) +"}"
	Play playString
END SUB

SUB WellPathMouseClickRight(dNorthing AS DOUBLE, dEasting AS DOUBLE)
	Dim playString AS STRING
	playString = "{ CLICK " + str(int((dEasting*3.6)+636)) + "," + str(int(720 - (dNorthing*3.6))) +", RIGHT }"
	Play playString
END SUB


SUB AddWellPathByHand(dNorthing AS DOUBLE, dEasting AS DOUBLE)
	WTreeItemClk("@2", "Horizon\Wellpaths", VK_RBUTTON)
	WMenuSelect("&Vertical wellpath")
	WellPathMouseClick(dNorthing,dEasting)
	' Now check the values returned 
	DIM ret as WELLPATH_INFO
	ret = GetWellPathInfo("New Wellpath")
	if ret.dNorthing < 89 or ret.dNorthing > 91 then
		tnofail "Northing failed to comply"
	endif
	if ret.dEasting < 29 or ret.dEasting > 31 then
		tnofail "Easting failed to comply"
	endif
	if ret.dDerrickElevation <> 0 then
		tnofail "Derrick Elevation failed to comply"
	endif
END SUB

SUB AddWellPathByValue(dNorthing as DOUBLE, dEasting AS DOUBLE)
	WTreeItemClk("@2", "Horizon\Wellpaths", VK_RBUTTON)
	WMenuSelect("&Vertical wellpath")
	WellPathMouseClickRight(dNorthing,dEasting)
	WMenuSelect("Enter numerical values")
	WEditSetText("@1",str(dNorthing))
	WEditSetText("@2",str(dEasting))
	WButtonClick("OK")
	DIM ret as WELLPATH_INFO
	ret = GetWellPathInfo("New Wellpath")
	if ret.dNorthing <> dNorthing then
		tnofail "Tried to set Northing to:" + str(dNorthing) +" result was: " + str(ret.dNorthing)
	endif
	if ret.dEasting <> dEasting then
		tnofail "Tried to set Northing to:" + str(dEasting) +" result was: " + str(ret.dEasting)
	endif

END SUB

FUNCTION ModifyWellPathByHand(sName AS STRING, dNorthing AS DOUBLE, dEasting AS DOUBLE, bAllowFail = 0 AS INTEGER) as STRING
	Dim failString as String
	if SelectItem("@2","Horizon\WellPaths\" + sName) then
		WTreeItemClk("@2", "Horizon\WellPaths\" + sName, VK_RBUTTON)
		WMenuSelect("Change Position")
		WellPathMouseClick(dNorthing,dEasting)
		' Now check the values returned 
		DIM ret as WELLPATH_INFO
		ret = GetWellPathInfo(sName)
		if ret.dNorthing < (dNorthing - 1) or ret.dNorthing > (dNorthing + 1) then
			failString = "Northing failed to comply"
		endif
		if ret.dEasting < (dEasting - 1) or ret.dEasting > (dEasting + 1) then
			failString = "Easting failed to comply"
		endif
		if bAllowFail = 0 and len(failString) > 0 then
			tnofail failString
		endif
	endif
	ModifyWellPathByHand = failString
	
END SUB

SUB ModifyWellPathByValue(sName AS STRING, dNorthing AS DOUBLE, dEasting AS DOUBLE)
	if SelectItem("@2","Horizon\WellPaths\" + sName) then
		WTreeItemClk("@2", "Horizon\WellPaths\" + sName, VK_RBUTTON)
		WMenuSelect("Change Position")
	endif
	WellPathMouseClickRight(dNorthing,dEasting)
	WMenuSelect("Enter numerical values")
	WEditSetText("@1",str(dNorthing))
	WEditSetText("@2",str(dEasting))
	WButtonClick("OK")
	DIM ret as WELLPATH_INFO
	ret = GetWellPathInfo(sName)
	if ret.dNorthing <> dNorthing then
		tnofail "Tried to set Northing to:" + str(dNorthing) +" result was: " + str(ret.dNorthing)
	endif
	if ret.dEasting <> dEasting then
		tnofail "Tried to set Northing to:" + str(dEasting) +" result was: " + str(ret.dEasting)
	endif

END SUB

' This function tests whether the reading of a deviated wellpath file succeeds
SUB AddDeviatedWellPath(sFileName AS STRING, sName AS STRING)
	WTreeItemClk("@2", "Horizon\Wellpaths", VK_RBUTTON)
	WMenuSelect("&Deviated wellpath")
	WEditSetText("File &name:", sFileName)
	WButtonClick("&Open")
	' After opening the file a wellpath with name sName must exist
	if not SelectItem("@2","Horizon\WellPaths\" +sName) then
		tnofail "Wellpath " + sName + " does not exist"
	endif

END SUB

SUB DumpWellPath(nFileId As INTEGER, sName AS STRING)
	Dim wellInfo AS WELLPATH_INFO
	wellInfo = GetWellPathInfo(sName)
	print #nFileId, "Wellpath: " + wellInfo.sName
	print #nFileId, "    Northing: " + str(wellInfo.dNorthing)
	print #nFileId, "    Easting : " + str(wellInfo.dEasting)
	print #nFileId, "    Derrick : " + str(wellInfo.dDerrickElevation)

END SUB

SUB DeleteWellPath(sName AS STRING)
	WTreeItemClk("@2", "Horizon\Wellpaths\" + sName, VK_RBUTTON)
	WMenuSelect("&Delete")
	' Now verify
	Dim wps AS ARRAY OF STRING
	wps = WellPaths()
	if not isempty(wellps) then
		Dim counter AS INTEGER
		for counter = lbound(wps) to ubound(wps)
			if wps(counter) = sName then
				tnofail "WellPath " + sName + " was not deleted"
			endif
		next
	endif
END SUB


'$endif