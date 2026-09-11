@ECHO OFF
IF "%1"=="" GOTO MISSING.ARGUMENT
SET OLDPATH=%PATH%
PATH=%QTDIR%\bin;%PATH%
PATH=%DIAROOT%\%1\bin;%PATH%
PATH=..\..\..\UserDLLs\x64;%PATH%
RMDIR /S/Q D0.copy
XCOPY /S/E/I D0 D0.copy
CD D0.copy
ATTRIB -R /S *
CALL ../test_single_geomec_rgi.bat %1 3 RockmechDS04
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Square5Model
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem33_rgi_ds_0 CBMtest.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 RockmechDS01_rgi_ds_0 flowSRMS_coarse_D1.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem33_rgi CBMtest_D0.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem07 Gem07.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem07_mrs_1_restart_1_rgi Gem07.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem44_rgi SingleHorizon.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 RockmechDS27_rgi RockmechDS27_export.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
CALL ../test_single_geomec_rgi.bat %1 3 Gem28_rgi Geomodel4.gm4
IF ERRORLEVEL 1 GOTO TEST.FAILED
:TEST.SUCCEEDED
@ECHO "TEST SUCCEEDED"
GOTO END
:TEST.FAILED
@ECHO "TEST FAILED"
GOTO END
:END
PATH=%OLDPATH%
SET OLDPATH=
CD ..
GOTO:EOF
:MISSING.ARGUMENT
@ECHO "missing argument (Debug|Release)"
