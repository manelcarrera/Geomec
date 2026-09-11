 '=====================================================================
' TEST CASE NAME:
'
' GENERAL DESCRIPTION OF WHAT IS VERIFIED:
'
'
' SPECIFIC AREA ADDRESSED BY THIS TEST CASE:
'
'
' NAME OF APPLICATION UNDER TEST:
'
'=====================================================================
' TEST CASE PURPOSE:
'
'
' TEST CASE ABSTRACT OF TECHNIQUES USED TO TEST THE FEATURE:
'
'
' SCENARIOS:    1.
'               2.
'               3.
'               4.
'               5.
'               6.
'               7.
'               8.
'               9.
'              10.
'
'---------------------------------------------------------------------------
' OTHER FILES REQUIRED TO RUN TEST CASE:
'
'
'
'---------------------------------------------------------------------------
' REVISION HISTORY:
'
'	[ 0]	<DD-MM-YY>		<NAME>	: <CHANGES MADE>
'	[ 1]	  -  -			<????>	: Created test.
'---------------------------------------------------------------------------


' '$DEFINE and '$INCLUDE metacommands for test case
'-------------------------------------------------------------------
'$include 'Recorder.inc'
'$include '..\AppSettings.inc'
'$include '..\AppControl.inc'


' DIM statements, TYPE declarations, global variables,
' constants, and so on for test case
'-------------------------------------------------------------------


' *************************
' **** BEGIN TEST CASE ****
' *************************

'******* Test Case INITIALIZATION/SETUP *********
Viewport Clear
TestName$ = "Test1.gm3"

'******* SCENARIO SECTION ***********************

'*** Scenario 1:
Scenario ("Test X")

StartApp								'Start application
LoadModel(CurDir$, TestName$)			'Load model test X
	
		
Scenario Cleanup 'and handling of scenario failure

ExitNoSave								'Exit Application
	
End Scenario


'******* Test Case CLEANUP **********************





' *************************
' ***** END TEST CASE *****
' *************************
End



'*****************************************************************************
'                          LOCAL SCRIPT SUBS/FUNCTIONS
'*****************************************************************************
