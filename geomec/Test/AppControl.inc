' File name: AppControl.inc
' Project:  Geomec Test Suite

' First include setting delivered by AppSetting.inc

'$include 'AppSettings.inc'
'$include 'WinApi.inc'
'$include 'Logging.inc'

const APPTAG = "Geomec3"
const STILL_ACTIVE = 259

'$IFDEF RELEASE
const BUILDTYPE = "Release"
'$ELSE
const BUILDTYPE = "Debug"
'$ENDIF

GLOBAL hProcess as Long

SUB StartApp
	DIM cwd As String
	DIM relpath As String
	DIM abspath As String
	DIM found As Integer
	DIM i As Integer
	DIM ret As Long

	DIM env As Long

	DIM si As STARTUPINFO
	DIM pi As PROCESS_INFORMATION

	cwd = CURDIR
	relpath = cwd

	found = 0
	for i = 1 to len(cwd)
		relpath = right(cwd, i)
		relpath = ucase(relpath)
		if instr(relpath, ucase(APPTAG)) = 1 then
			found = 1
			abspath = left(cwd, len(cwd) - i)
			exit for
		endif
	next

	if found = 0 then
		tnofail "Application tag " + APPTAG + " not found in current directory: " + cwd
	endif

	abspath = abspath + APPTAG + "\" + BUILDTYPE + "\" + APP_EXENAME

	si.cb = len(si)

	' explicitly copy environment to child process
	env = GetEnvironmentStrings()

	ret = CreateProcess( _
	  abspath, _                ' name of executable module
	  NULL, _                   ' command line string
	  NULL, _                   ' SD
	  NULL, _                   ' SD
	  true, _                   ' handle inheritance option
	  NORMAL_PRIORITY_CLASS, _  ' creation flags
	  env, _                   ' new environment block
	  cwd, _                    ' current directory name
	  si, _                     ' startup information
	  pi)                       ' process information

	if ret = false then
		tnofail "CreateProcess failed"
	endif

	hProcess = pi.hProcess

	IF WFndWnd("- GEOMEC 3", FW_ACTIVE_ONLY or FW_PART, 10) = 0 THEN
		tnoFail APP_NAME + " main window is not found after start up"
	End If

	' Make sure window is at 1280x1024 in upper left corner for resolution dependent tests
	if WIsMaximized(hwnd) then
		WResWnd(0)
	endif
	WSetWndPos(0, 0, 0)       ' upper left corner
	WSetWndSiz(0, 1280, 1024) ' 1280x1024 size
END SUB

SUB KillApp
	DIM exitcode As Long
	GetExitCodeProcess(hProcess, varptr(exitcode))
	if exitcode = STILL_ACTIVE then
		print "KillApp: Terminating process"
		TerminateProcess(hProcess, -1)
	endif
END SUB

SUB WaitForProgress
	dim info as INFO
	dim ready as long
	dim hwnd as HWND

	do
		hwnd = WFndWnd("- GEOMEC 3", FW_PART or FW_ACTIVE_ONLY or FW_NOERROR)
		if(hwnd) then
			wgetinfo(hwnd, info)
			ready = info.Enabled
		else
			ready = 0
		endif
		Sleep(0.1)
	loop while ready = 0
END SUB

SUB ImportGM3Model(Path As STRING, FileName As STRING)
	print "Importing file " + Path + "\" + filename
	WMenuSelect("&File\&Open Model...")	
	CurrentWindow = WFndWndC("Open", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WComboItemClk("Files of &type:", "GEOMEC 2 Databases (*.gm3)")
	WEditSetText("File &name:", Path + "\" + FileName)
	WButtonClick("&Open")
'	if WFndWnd("- GEOMEC 3", FW_ACTIVE_ONLY or FW_PART, 120) = 0 THEN
'		WFndWndC("Geomec", "#32770", FINDWINDOWFLAGS_IF , Timeout)
'		WButtonClick("OK")		' Click OK button 
'	END IF
	WaitForProgress
END SUB

SUB LoadModel(Path As STRING, FileName As STRING)
	WMenuSelect("&File\&Open Model...")
	CurrentWindow = WFndWndC("Open", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 355, 320, 426, 264)
	Play "{Click 508, 534, Left}"
	Play Path + "\" + FileName

	WButtonClick("&Open")
	if WFndWnd("- GEOMEC 3", FW_ACTIVE_ONLY or FW_PART, 90) = 0 THEN
		WFndWndC("Geomec", "#32770", FINDWINDOWFLAGS_IF , Timeout)
		WButtonClick("OK")		' Click OK button 
		
	END IF
'	WFndWndC(FileName + " - GEOMEC 2D", "#32770", FINDWINDOWFLAGS_IF, 10)
END SUB

SUB SaveModel(Path As STRING, FileName As STRING)
	WMenuSelect("&File\Save Model &As...")
	if WFndWnd("Save As", FW_PART,10) <> 0 then
		Play "{Alt}" + "n"
		Play Path + "\" + FileName
		
		WButtonClick("&Save")
	endif


END SUB

SUB NewModel(ModelName As STRING)
	WFndWnd("- GEOMEC 3" , FW_IGNOREFILE_LEFT, 2)
	WMenuSelect("&File\&New Model")
	CurrentWindow = WFndWndC("New model", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	IF ModelName = "" THEN
		WButtonClick("Cancel")
	ELSE
		WViewItemClk("@1", ModelName, VK_LBUTTON)
		WButtonClick("OK")
	ENDIF

	CurrentWindow = WFndWnd("Untitled - GEOMEC 3", FW_IGNOREFILE Or FW_ERROR, Timeout)
END SUB

FUNCTION GetFileNameNoExt(Path As STRING) AS STRING
	DIM sFileName As STRING
	DIM Pos As Integer
	sFileName = GetFileName(Path)
	Pos = 0
	while NOT(Instr(Pos + 1, sFileName, ".") = 0)
		Pos = Instr(Pos + 1, sFileName, ".")
	wend

	GetFileNameNoExt = Left$(sFileName, Pos - 1)
END SUB

FUNCTION GetFileName(Path As String) As String
	DIM Pos As Integer
	Pos = 0
	while NOT(Instr(Pos + 1, Path, "\") = 0)
		Pos = Instr(Pos + 1, Path, "\")
	wend

	GetFileName = Right$(Path, Len(Path) - Pos)
END FUNCTION
		
FUNCTION GetPathName(Path As String) As String
	DIM Pos As Integer
	Pos = 0
	while NOT(Instr(Pos + 1, Path, "\") = 0)
		Pos = Instr(Pos + 1, Path, "\")
	wend

	GetPathName = Left$(Path, Pos - 1)
END FUNCTION

SUB PreCalc
	WMenuSelect("&Module\&Mesh")
	WMenuSelect("&Module\&Post Processing", 30)
	Sleep(.1)
'	CurrentWindow = WFndWnd("Calculating",  FINDWINDOWFLAGS_IF, Timeout)
	
	WButtonClick("OK", 160)
END SUB

SUB LinCalc
	WMenuSelect("&Analysis\&Linear Depletion")

	WaitForProgress
END SUB

SUB ExitNoSave
	if (WFndWnd("Application Error", FW_PART, 10) <> 0) or (WFndWnd("Microsoft Visual C++ Runtime Library", FW_PART, 10) <> 0) then
		print "ExitNoSave: Recovering from crash"
		KillApp
		wbuttonclick("OK")
		exit sub
	endif

	if WFndWnd("Microsoft Visual C++ Debug Library", FW_PART, 10 <> 0) then
		print "ExitNoSave: Recovering from debug assertion"
		wbuttonclick("Abort")
		exit sub
	endif

  	WMenuSelect("&File\E&xit")
	' To do click no save
'	if WFndWnd("Save changes to", FW_ACTIVE or FW_NOCASE or FW_CHILDOK or FW_PART or FW_DIALOG, 30) <> 0 THEN
	if WFndWnd("GEOMEC 3", FW_NOCASE or FW_CHILDOK or FW_PART, 30) <> 0 THEN
		WButtonClick("&No")		' Click OK button 
	END IF
	if WFndWnd("Save As", FW_PART, 10) <> 0 then
		WButtonClick("Cancel")
	endif
	sleep 2
END SUB

SUB SaveAndExit
	WMenuSelect("&File\&Save Model")
	if WFndWnd("Save As", FW_PART,10) <> 0 then
		WButtonClick("&Save")
	endif
	Sleep(5)
	WMenuSelect("&File\E&xit")
	Sleep(3) ' wait a little while
	DIM exitcode As Long
	GetExitCodeProcess(hProcess, varptr(exitcode))
	if exitcode = STILL_ACTIVE then
		' The application is presumably asking for save. So Send 'No' 
		if WFndWnd("Geomec 3",FW_PART,10) <> 0 then
		WButtonClick("&No")
		endif
		tnofail "Save and exit failed because the application remained active"
	endif

END SUB

SUB CompLegend( sFileName As String, sScreenName As String )
	' Size of legend screen
	Dim LegendReg As RECT
	LegendReg.wLeft = 239 
	LegendReg.wTop = 64
	LegendReg.wRight = 397
	LegendReg.wBottom = 816

	IF RESULT_RECORD THEN
		ScnCaptureScreen(sFileName, sScreenName, LegendReg, FALSE)
	ELSE
		ScnCompScreen (sFileName, sScreenName, LegendReg, FALSE, TRUE)
	END IF
END SUB

SUB CompModel( sFileName As String,	sScreenName As String )
	' Size of model screen
	Dim ModelReg As RECT
	ModelReg.wLeft = 404 
	ModelReg.wTop = 64
	ModelReg.wRight = 1150
	ModelReg.wBottom = 816

	IF RESULT_RECORD THEN
		ScnCaptureScreen(sFileName, sScreenName, ModelReg, FALSE)
	ELSE
		ScnCompScreen (sFileName, sScreenName, ModelReg, FALSE, TRUE)
	END IF
END SUB

SUB XResComp( sResult As STRING, _
			  sStage As STRING, _
			  sPath As STRING)
	' Wait for main window ...
'	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)

	' Select result
	WTabItemClk("#12320", "X-Section", 30)
	WTreeItemClk("#1176", sResult, VK_LBUTTON)
	WTreeItemClk("#1176", sStage, VK_LBUTTON)	

	' Wait for main window ...
	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)

	' Path
	sName$ = "XSEC " + sResult + " " + sStage + " "

	' Compare ....
	CompModel( sPath + "\ResultCapture.scn",sName + "Model" )
	CompLegend( sPath + "\ResultCapture.scn",sName + "Legend" )
END SUB

SUB PresResComp( sStage As STRING, _
			  bSubstractInitial As LONG, _
			  sPath As STRING )
	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)

	WMenuSelect("&Module\&Geology")
	WComboItemClk("Pressures", sStage)

	sScreenLabel$ = "Pressures " + sStage 

	IF WComboEnabled("#1268") THEN
		IF bSubInitial THEN
			WComboItemClk("#1268", "Substract initial pressures")
			sScreenLable$ = sScreenLabel$ + " Substract Initial"
		ELSE
			WComboItemClk("#1268", "Pressures only")
		END IF
	END IF

	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)

	CompModel( sPath + "\ResultCapture.scn",sScreenLabel + " Model" )
	CompLegend( sPath + "\ResultCapture.scn",sScreenLabel + " Legend" )
END SUB

SUB CreateHorizonFromSurface( sSurface As STRING )
	' Select surface
	WTreeItemClk("@1", "Data Storage\Surfaces\" + sSurface, VK_RBUTTON)
	WMenuSelect("Create horizon")

	' Check horizon
'	WTreeItemExists("@1", "Horizon Model
END SUB

SUB InsertWellPath
	' Wait for main window ...
	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)

	WMenuSelect("&Insert\&Well Path...")

	CurrentWindow = WFndWndC("Well Path position", "#32770", FINDWINDOWFLAGS_IF, Timeout)
	WSetWndPosSiz(CurrentWindow, 285, 146, 581, 568)
	Play "{BtnDown 647, 205, Left}"
	Play "{BtnUp 661, 586, Left}"
'	Sleep(8.863)
'	WEditSetSel("@1", 1, 9)
'	WEditSetSel("@1", 6, 4)
'	WEditSetSel("@1", 6, 0)
'	Play "4"
'	Play "{BS}"
'	WEditSetSel("@1", 2, 0)
'	Play "{BtnDown 378, 647, Left}"
'	Play "{BtnUp 437, 647, Left}"
'	WEditSetSel("@1", 1, 9)
'	Play "4"
'	Play "00"
'	Play "{Click 402, 674, Left}"
'
'	CurrentWindow = WFndWndC("Geomec2D", "#32770", FINDWINDOWFLAGS_IF, Timeout)
'	WSetWndPosSiz(CurrentWindow, 429, 371, 293, 119)
'	WButtonClick("OK")
'	Sleep(6.8)
'
'	CurrentWindow = WFndWndC("Well Path position", "#32770", FINDWINDOWFLAGS_IF, Timeout)
'	WSetWndPosSiz(CurrentWindow, 285, 146, 581, 568)
'	WEditSetSel("@2", 1, 9)
'	Play "{Click 661, 393, Left}"
'	Play "{BtnDown 661, 393, Left}"
'	Play "{BtnUp 640, 393, Left}"
'	Sleep(4.937)
'	Play "{BtnDown 640, 491, Left}"
'	Play "{BtnUp 603, 491, Left}"
'	Play "{Click 672, 491, Left}"
'	Play "{BtnDown 380, 649, Left}"
'	Play "{BtnUp 438, 645, Left}"
'	Play "20"
'	Play "{Click 374, 683, Left}"
'	WEditSetSel("@1", 8, 2)
'	WEditSetSel("@1", 7, 3)
'	WEditSetSel("@1", 1, 9)
'	Play "{Click 378, 647, Left}"
'	Play "3"
'	Play "0"
'	Play "{DELETE}{DELETE}"
'	Play "{Click 424, 665, Left}"
'	WEditSetSel("@2", 9, 0)
'	WEditSetSel("@2", 6, 0)
'	Sleep(7.410)
'	Play "{Click 381, 675, Left}"
'	WEditSetSel("@2", 4, 0)
'	Play "{RIGHT}"
	WButtonClick("OK")
	WFndWnd(" - GEOMEC 2D",FW_DEFAULT OR FW_IGNOREFILE_LEFT, 30)
END SUB
