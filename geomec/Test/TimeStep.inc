' File name: TimeStep.inc
' Project:  Test
'$ifndef TIMESTEP_INC
'$define TIMESTEP_INC

TYPE TIMESTEP_INFO
	sName As STRING
	nYear As INTEGER
END TYPE

FUNCTION GetTimeStepInfo(sName As STRING) As TIMESTEP_INFO
'Open dlg

'Find time step

'Retrieve info

END FUNCTION

FUNCTION GetTimeStepCount()
	WTreeItemClk("@2", "Horizon", VK_RBUTTON)
	WMenuSelect("Attributes")
	CurrentWindow = WFndWndC("Model attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 449, 337, 382, 323)
	WTabItemClk("@1", "Depletion Stages")
	GetTimeStepCount = val(EditText("Number of depletion stages:"))
	WButtonClick("Cancel")
END FUNCTION

SUB SetTimeStepCount(nTimeStep As INTEGER)
	' Open TimeStep window and select depletion stage tab
	WTreeItemClk("@2", "Horizon", VK_RBUTTON)
	WMenuSelect("Attributes")
	CurrentWindow = WFndWndC("Model attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 449, 337, 382, 323)
	WTabItemClk("@1", "Depletion Stages")
	' Modify nr of depletion stage and dismiss
	WEditSetText("Number of depletion stages:", STR(nTimeStep), 0)
	WButtonClick("OK")	

	' Open TimeStep window and select depletion stage tab
	WTreeItemClk("@2", "Horizon", VK_RBUTTON)
	WMenuSelect("Attributes")
	CurrentWindow = WFndWndC("Model attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 449, 337, 382, 323)
	WTabItemClk("@1", "Depletion Stages")
	' Check nr of depletion stage and dismiss
	RecVerifyText("Number of depletion stages:", LTRIM(STR(nTimeStep)), 0, V_EDIT)
	WButtonClick("OK")	
END SUB

SUB DumpTimeStep(nFileId As INTEGER)
	'Get the time step tab
	WTreeItemClk("@2", "Horizon", VK_RBUTTON)
	WMenuSelect("Attributes")
	CurrentWindow = WFndWndC("Model attributes", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 449, 337, 382, 323)
	WTabItemClk("@1", "Depletion Stages")

	'Get the nr of depletion stages
	PRINT #nFileId, "***	TimeSteps	***"
	PRINT #nFileId, "Nr of Timesteps : "; EditText("@1")
	PRINT #nFileId,"" 

	'Get the list control size
	DIM ret AS ARRAY OF STRING
	DIM item AS LONG
	REDIM ret(1 TO WViewCount("@1")) As STRING
	For item = 1 to WViewCount("@1")
	PRINT #nFileId, "Name : "; ViewItemText("@1", _ord(item), "@1")
	PRINT #nFileId, "Year : "; ViewItemText("@1", _ord(item), "@2") 
	Next item

	'Dismiss dialog
	WButtonClick("OK")
END SUB

'$endif