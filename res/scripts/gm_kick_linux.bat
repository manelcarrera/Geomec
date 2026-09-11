rem This script is placed on the build server locally, to avoid security risks through code injection.
rem -----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----8<-----
@echo on

setlocal

set workdir=%1
set tfsbuild=%2
set configuration=%3
set build_type=%4


set user=EDIT_ME
set pkey=EDIT_ME
set host=amsd2a-n-b01901.europe.shell.com

set PATH="C:\Program Files (x86)\PuTTY";"E:\Apps\Microsoft Visual Studio 10.0\Common7\IDE";%PATH%

echo cd %workdir%
cd %workdir%

echo plink.exe -batch -ssh -2 -l %user% -i %pkey% %host% "/bin/bash script/autobuild release %build_type%"
plink.exe -batch -ssh -2 -l %user% -i %pkey% %host% "/bin/bash script/autobuild release %build_type%" > autobuild.tmp

set /p lnxbuild= < autobuild.tmp

mkdir %lnxbuild%

echo lcd %lnxbuild% > autoget.tmp
echo cd drops/%lnxbuild% >> autoget.tmp
echo mget * >> autoget.tmp
echo quit >> autoget.tmp

echo psftp.exe -batch -l %user% -i %pkey% %host% -b autoget.tmp
psftp.exe -batch -l %user% -i %pkey% %host% -b autoget.tmp

echo mstest.exe /publishresultsfile:%lnxbuild%\test_output.trx /publish:https://sww-sede-team.shell.com/tfs/PTSSW /TeamProject:Geomec /platform:x64 /flavor:%configuration% /publishbuild:%tfsbuild%
mstest.exe /publishresultsfile:%lnxbuild%\test_output.trx /publish:https://sww-sede-team.shell.com/tfs/PTSSW /TeamProject:Geomec /platform:x64 /flavor:%configuration% /publishbuild:%tfsbuild%

endlocal