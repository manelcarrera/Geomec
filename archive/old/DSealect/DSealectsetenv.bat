rem -- Default to development                 --
rem -- Use e.g. diana93 for released versions --
rem -- e.g. set DIAVER=diana93                --
    if "_%DIAVER%_" == "__" set DIAVER=diana

rem -- Development root directory             --
    if not "_%DEVELOP%_" == "__" goto DevelopSet

    echo.
    echo DEVELOP variable is automatically set...
rem -- Assume the batch file is located in the geomec3 subdirectory under the intended DEVELOP root
rem -- %~dp0 expands to the full path of the directory in which this file is located
    set CURRENTDIR=%~dp0
rem -- Remove the last 9 characters from the path (\geomec3\)
    call ..\geomec3\getparent %CURRENTDIR%
    set DEVELOP=%PARENT_FOLDER%
    call ..\geomec3\getparent %DEVELOP%
    set DEVELOP_PARENT=%PARENT_FOLDER%

rem -- Read DIANA library environment variables
    call %DEVELOP%\dialibsenv.bat

:DevelopSet

rem -- Servers                                --
rem -- Using full domain names for VPN users  --
    set DIA32SVR=ijsseldijk.diana.local
    set DIA64SVR=speelpenning.diana.local
    set DIASHARESVR=srv-nfs.diana.local

rem -- Determine architecture                 --
    if "_%PROCESSOR_ARCHITECTURE%_" == "_AMD64_" goto IsAMD64

    echo.
    echo *** 32 bits architecture ***
    set OLECTRA=C:\Olectra
    set DIASVR=%DIA32SVR%
    set USERDLLS=%DEVELOP%\UserDlls\Debug;%DEVELOP%\UserDlls
    set FLEXLM=%DEVELOP%\geomec3\FlexLM
    set INTEL=C:\dev\Intel\Compiler\11.1\065\bin\ia32

rem -- Tools paths 32 bits                    --
    set QTDIR=C:\Qt\4.8.3
    set QWTDIR=C:\qwt\qwt-5.2
    set WixToolPath=C:\Wix3\bin

    goto SetEnvironment

:IsAMD64
    echo.
    echo *** 64 bits architecture ***
    set DIASVR=%DIA64SVR%
    set USERDLLS=%DEVELOP%\UserDlls\x64\Debug;%DEVELOP%\UserDlls\x64
    set FLEXLM=%DEVELOP%\geomec3\FlexLM\x64
    set INTEL=C:\dev\Intel\Compiler\11.1\065\bin\intel64

rem -- Tools paths 64 bits                    --
    set QTDIR=C:\Qt\4.8.3
    set QWTDIR=C:\qwt\qwt-5.2
    set WixToolPath=C:\Wix3\bin


:SetEnvironment
    if "_%DIAVER%_" == "_diana_" goto DevVersion
    echo.
    echo *** Diana release version (%DIAVER%) ***
    set DIASHARE=\\%DIASHARESVR%\%DIAVER%share
    goto ShareDone

:DevVersion
    echo.
    echo *** Diana development version ***
    set DIASHARE=\\%DIASHARESVR%\diashare

:ShareDone
    echo.
    set DIAPATH=\\%DIASVR%\%DIAVER%\release
    set DIAOBJ=%DIAPATH%\lib
    set DIASRC=%DIASHARE%\src
    set DIAOBJINCLUDE=%DIAPATH%\src\include
    set DIASRCINCLUDE=%DIASRC%\include

    set DSPPATH=\\%DIA32SVR%\users\all\dsp
    set DSPLIB=%DSPPATH%\lib

    set FWDIR=%DEVELOP_PARENT%\fw51

    set PATH=%USERDLLS%;%DSPLIB%;%QTDIR%\bin;%QWTDIR%\lib;%DIAPATH%\bin;%FLEXLM%\debug;%INTEL%;%PATH%

    echo.
    echo DEVELOP=%DEVELOP%
    echo DIAPATH=%DIAPATH%
    echo DIASHARE=%DIASHARE%
    echo FWDIR=%FWDIR%
    echo PATH=%PATH%
    echo.

