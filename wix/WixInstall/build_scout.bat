@echo off

setlocal

call ..\geomecsetenv.bat

%WIXTOOLPATH%\candle.exe -dIsWin64=yes -out obj64\Release\WixInstallShared.wixobj WixInstallShared.wxs
%WIXTOOLPATH%\Light.exe -out bin\Release\GEOMEC3_64.msi -sw1055 -sw1076 obj64\Release\WixInstallShared.wixobj

endlocal
