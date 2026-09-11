'$IFNDEF LOGGING_INC
'$DEFINE LOGGING_INC

' File name: Logging.inc
' Project:  Test

SUB StartLog(sFile AS STRING,sDescription AS STRING)
	OPEN sFile FOR OUTPUT AS #20
	PRINT #20,"Start Log for " + sDescription
END SUB

SUB StartScenario(sDescription AS STRING)
	PRINT #20,"Scenario : " + sDescription + " started"
END SUB

SUB ScenarioPass
	PRINT #20,"Scenario Passed the test"
END SUB

SUB TNOFAIL(sReason AS STRING)
	PRINT #20,"Fail: " + sReason
	Fail sReason
END SUB

SUB EndLog
	CLOSE #20
END SUB

'$ENDIF