

rem set PATH="E:\APPS\Microsoft Visual Studio 10.0\Team Tools\Performance Tools\x64";%PATH%

rem vsinstr /coverage %1

@echo on

setlocal

attrib %AppData%\it -h

del %AppData%\it

endlocal
