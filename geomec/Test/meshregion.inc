'$IFNDEF MESHREGION_INC
'$DEFINE MESHREGION_INC

' File name: Meshregion.inc
' Project:  Test


TYPE MESHREGION_INFO
	iGridSizeX AS INTEGER
	iGridSizeY AS INTEGER
END TYPE

SUB MeshregionStartup
	StartApp
	LoadModel(curdir$,"test.gmp")
	Sleep(5)
END SUB

FUNCTION GetMeshRegionInfo(sName As STRING) As MESHREGION_INFO
	if WFndWnd("- GEOMEC 3", FW_ACTIVE_ONLY or FW_PART, 60) THEN
		DIM ret as MESHREGION_INFO
		WTreeItemClk("@2", "Horizon\Mesh\" + sName, VK_RBUTTON)
		WMenuSelect("Attributes")
		Sleep(0.1)
		if WEditExists("#1215") then
			ret.iGridSizeX = val(EditText("#1215"))
		else
			ret.iGridSizeX = val(ComboText("@1"))
		endif
		if WEditExists("#1216") then
			ret.iGridSizeY = val(EditText("#1216"))
		else
			ret.iGridSizeY = val(ComboText("@2"))
		endif
		WButtonClick "Cancel"

		GetMeshRegionInfo = ret
	endif

END FUNCTION

SUB DumpMeshRegion(nFileId As INTEGER, sName AS STRING)
	DIM meshinfo AS MESHREGION_INFO
	meshInfo = GetMeshRegionInfo(sName)
	print #nFileId, "MeshRegion: " + sName
	print #nFileId, "    Gridsize X: " + str(meshInfo.iGridSizeX)
	print #nFileId, "    Gridsize Y: " + str(meshInfo.iGridSizeY)

END SUB

SUB SetMainRegion(X_GRID AS INTEGER, Y_GRID AS INTEGER)
	' Set main grid
	WTreeItemClk("@2", "Horizon\Mesh\Main", VK_RBUTTON)
	WMenuSelect("&Attributes")
	Sleep(0.1)
	CurrentWindow = WFndWndC("Main mesh region", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WEditSetText("#1215", STR(X_GRID))
	WEditSetText("#1216", STR(Y_GRID))
	WButtonClick("OK")

	' Verify main grid
	WTreeItemClk("@2", "Horizon\Mesh\Main", VK_RBUTTON)
	WMenuSelect("&Attributes")
	Sleep(0.1)
	CurrentWindow = WFndWndC("Main mesh region", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	RecVerifyText("#1215", LTRIM(STR(X_GRID)), 0, V_EDIT)
	RecVerifyText("#1216", LTRIM(STR(Y_GRID)), 0, V_EDIT)
	WButtonClick("OK")
END SUB

TYPE MESHPOINT
	x AS INTEGER
	y AS INTEGER
END TYPE

' This function assumes a maximized application with default layout and already a topview selected
SUB MeshPointClick(x AS INTEGER, y AS INTEGER)
	Dim playString AS STRING
	playString = "{ CLICK " + str(int((x*3.6)+636)) + "," + str(int(710 - (y*3.6))) +"}"
	Play playString
END SUB

SUB MeshPointRightClick(x AS INTEGER, y AS INTEGER)
	Dim playString AS STRING
	playString = "{ CLICK " + str(int((x*3.6)+636)) + "," + str(int(710 - (y*3.6))) +", RIGHT}"
	Play playString
END SUB


' This routine creates a new meshregion given the points defining it in the array. It will close the polygon for you so don't provide
' the first point as last point.
SUB CreateMeshRegion(points AS ARRAY OF MESHPOINT, xGrid AS DOUBLE, yGrid AS DOUBLE)
	WTreeItemClk("@2", "Horizon\Mesh", VK_RBUTTON)
	WMenuSelect("&New Meshregion")
	DIM counter AS LONG
	for counter = LBOUND(points) to UBOUND(points)
		MeshPointClick(points(counter).x,points(counter).y)
	next counter
	MeshPointRightClick(points(lbound(points)).x,points(lbound(points)).y)
	WMenuSelect("Ready")
	' We now expect a window allowing the user to set the gridsize
	' find the index for the item we have to select.
	for counter = 1 to WComboCount("@1")
		if val(ComboItemText("@1",counter)) = xGrid then
			WComboItemClk("@1",ComboItemText("@1",counter))
		endif
	next
	for counter = 1 to WComboCount("@2")
		if val(ComboItemText("@2",counter)) = yGrid then
			WComboItemClk("@2",ComboItemText("@2",counter))
		endif
	next
	' remember the name of the new region to check the values entered
	Dim RegionName AS STRING
	RegionName = EditText("@1")
	WButtonClick("OK")
	Dim meshInfo as MESHREGION_INFO
	meshInfo = GetMeshRegionInfo(RegionName)
	if meshInfo.iGridSizeX <> xGrid then
		tnofail "Grid size X must be " + str(xGrid) + " but it is set to " + str(meshInfo.iGridSizeX)
	endif
	if meshInfo.iGridSizeY <> yGrid then
		tnofail "Grid size Y must be " + str(yGrid) + " but it is set to " + str(meshInfo.iGridSizeY)
	endif
END SUB

SUB DeleteMeshRegion(sName AS STRING)
	WTreeItemClk("@2", "Horizon\Mesh\" + sName, VK_RBUTTON)
	WMenuSelect("&Delete")
	' Now verify
	Dim regions AS ARRAY OF STRING
	regions = MeshRegions()
	Dim counter AS INTEGER
	for counter = lbound(regions) to ubound(regions)
		if regions(counter) = sName then
			tnofail "Region " + sName + " was not deleted"
		endif
	next
END SUB

'$ENDIF