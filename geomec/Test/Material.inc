' File name: Material.inc
' Project:  Test

SUB NewMaterial(sMaterial As STRING)
	WTreeItemClk("@1", "Data Storage\Materials", VK_RBUTTON)
	WMenuSelect("Attributes")

	CurrentWindow = WFndWndC("Material Library", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 446, 218, 389, 561)
	WButtonClick("&New")


	CurrentWindow = WFndWndC("Material Name", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 498, 435, 285, 127)
	Play sMaterial
	WButtonClick("OK")


	CurrentWindow = WFndWndC("Material Parameters", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 149, 152, 982, 692)
	Play "{Click 202, 798, Left}"
	WButtonClick("Close")


	CurrentWindow = WFndWndC("Material Library", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 446, 218, 389, 561)
	WButtonClick("OK")
END SUB

SUB AssignMaterial(sMaterial As STRING, sFormation As STRING, sMaterialOld = "NONE" As STRING )
'	WTreeItemExpand("@1", "Materials")
	WTreeItemClk("@1", "Data Storage\Materials\" + sMaterial, VK_RBUTTON)
	WMenuSelect("&Copy")
'	WTreeItemExpand("@2", "Horizon\Formations\" + sFormation)
	WTreeItemClk("@2", "Horizon\Formations\" + sFormation + "\" + sMaterialOld, VK_RBUTTON)
	WMenuSelect("&Paste")
END SUB