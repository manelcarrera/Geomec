'$IFNDEF BREAKDOWN_INC
'$DEFINE BREAKDOWN_INC

SUB BreakdownStartup
	StartApp
	LoadModel(curdir$,"test.gmp")
	Sleep(5)
END SUB

'$ENDIF
