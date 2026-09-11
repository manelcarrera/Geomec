' File name: Boundary.inc
' Project:  Test

SUB SetUserDefinedBoundary(dMinEast AS DOUBLE, dMinNorth AS DOUBLE, dMaxEast AS DOUBLE, dMaxNorth AS DOUBLE)
	WTreeItemClk("@2", "Horizon\Boundary", VK_RBUTTON)
	WMenuSelect("&Attributes")
	Sleep(0.1)
	CurrentWindow = WFndWndC("Boundary", "#32770", FINDWINDOWFLAGS_IF, Timeout)

	' Select User defined
	WOptionSelect("#1037")

	WEditSetText("&Easting", STR(dMinEast))
	WEditSetText("&Northing", STR(dMinNorth))
	WEditSetText("Ea&sting", STR(dMaxEast))
	WEditSetText("No&rthing", STR(dMaxNorth))

	WButtonClick("OK")

	'Verify

	WTreeItemClk("@2", "Horizon\Boundary", VK_RBUTTON)
	WMenuSelect("&Attributes")
	Sleep(0.1)
	CurrentWindow = WFndWndC("Boundary", "#32770", FINDWINDOWFLAGS_IF, Timeout)

	' Select User defined
	RecVerifyEnabled("#1037", NULL, V_OPTION, TRUE)
	RecVerifyText("&Easting", LTRIM(STR(dMinEast)), 0, V_EDIT)
	RecVerifyText("&Northing", LTRIM(STR(dMinNorth)), 0, V_EDIT)
	RecVerifyText("Ea&sting", LTRIM(STR(dMaxEast)), 0, V_EDIT)
	RecVerifyText("No&rthing", LTRIM(STR(dMaxNorth)), 0, V_EDIT)

	WButtonClick("OK")
END SUB
