@echo off

net session >nul 2>&1

if %errorlevel% neq 0 (
	echo Please make sure that you run this script as Administrator

	goto :exit
)

pushd "%~dp0"

set RESDIR=.\resources
set HASPDIR=C:\Program Files (x86)\Common Files\Aladdin Shared

echo Verify (and possibly fix) Diana license settings

"%RESDIR%\haspdinst.exe" -i

if not exist "%HASPDIR%" mkdir "%HASPDIR%"
if not exist "%HASPDIR%\HASP" mkdir "%HASPDIR%\HASP"
if not exist "%HASPDIR%\HASP\lmid" mkdir "%HASPDIR%\HASP\lmid"
if not exist "%HASPDIR%\HASP\log" mkdir "%HASPDIR%\HASP\log"

"%RESDIR%"\WixHaspPostInstall

popd

:exit
pause
