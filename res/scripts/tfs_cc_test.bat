setlocal

cd "%1"

set PATH="E:\APPS\Microsoft Visual Studio 10.0\Team Tools\Performance Tools\x64";E:\APPS\Microsoft Visual Studio 10.0\Common7\IDE;..\src\Tests\scripts;%PATHPREFIX%;.;%PATH%


call tfs_instrument.bat tests.exe


vsperfcmd /start:coverage /output:tests.coverage

call tfs_test.bat ..\src\Tests\Models\ tests

vsperfcmd /shutdown


echo mstest /publishresultsfile:test_output.trx /publish:https://sww-sede-team.shell.com/tfs/PTSSW /TeamProject:Geomec /platform:x64 /flavor:%3 /publishbuild:%2
mstest /publishresultsfile:test_output.trx /publish:https://sww-sede-team.shell.com/tfs/PTSSW /TeamProject:Geomec /platform:x64 /flavor:%3 /publishbuild:%2


if "%4" == "kick_linux" (
  call C:\Users\GM-TFSBUILD-S\gm_kick_linux.bat %1 %2 %3 %5
)


endlocal
