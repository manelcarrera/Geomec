' File name: DataImport.inc
' Project:  Test
SUB Import(Path As STRING, FileName As STRING)
	WTreeItemClk("@1", "Data Storage\Point sets", VK_RBUTTON, 100)
	WMenuSelect("&Import...")

'	CurrentWindow = WFndWndC("Open", "#32770", FINDWINDOWFLAGS_IF, Timeout)

	IF(FileName = "") THEN
		WButtonClick("Cancel")
	ELSE
		Play Path + "\" + FileName
		WButtonClick("&Open")
	END IF
END SUB

SUB AttriPointSet(PointSet As String, Dimension As Integer)
	IF Dimension = 2 THEN
		WTreeItemClk("@1", "Data Storage\Point sets\2D\" + PointSet , VK_RBUTTON, 100)
	ELSE
		WTreeItemClk("@1", "Data Storage\Point sets\3D\" + PointSet , VK_RBUTTON, 100)
	END IF

	WMenuSelect("&Attributes")
	CurrentWindow = WFndWndC("Pointset Attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
END SUB

SUB NewProperty(Property As String)
	CurrentWindow = WFndWndC("Pointset Attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WCheckClick("#1151")
	Play Left(Property,1)
'	WMenuSelect(Property, 5)	
	Play "{Enter}"
end sub
