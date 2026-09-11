    @echo off

rem -- Keep environment variables local to this batch file
    setlocal

rem -- %1 is diana root dir
    if "%1" == "" goto Usage
rem -- %2 is diana share dir
    if "%2" == "" goto Usage
rem -- %3 is SafeNet root dir
    if "%3" == "" goto Usage
rem -- %5 is target dir
    if "%5" == "" goto Usage

    set DIAROOT=%1
    set DIASHARE=%2
    set SAFENET=%3
	set INTEL=%4
	set TARGET=%5

    if exist %TARGET%\diana rmdir /s /q %TARGET%\diana
    mkdir %TARGET%\diana

    call:binfiles %TARGET% release
    call:binfiles %TARGET% debug
    call:sharefiles %TARGET%

    goto End

:binfiles

rem REL is either 'release' or 'debug'
    set REL=%2
    mkdir %1\diana\%REL%

    set DIAPATH=%DIAROOT%\%REL%
    set DIALIB=%DIAPATH%\lib
    set DIABIN=%DIAPATH%\bin
    set BINSEG=%DIAPATH%\binseg

rem -- lib
    set SD=%DIALIB%
    set TD=%1\diana\%REL%\lib
    mkdir %TD%
    call:cff lbci30
    call:cff lbcp40
    call:cff lbcx40
    call:cff lbdc32
    call:cff lbds30
    call:cff lbel30
    call:cff lbfl51
    call:cff lbgc51classes
    call:cff lbgc51routines
    call:cff lbgs30
    call:cff ElmTypes
    call:cff SCHEMA

rem -- bin
    set SD=%DIABIN%
    set TD=%1\diana\%REL%\bin
    mkdir %TD%

    call:cff aplb40
    call:cff baxq41
    call:cff cilb01
    call:cff dclb33
    call:cff demf50
    call:cff deou50
    call:cff eiem40
    call:cff eiev50
    call:cff eiou40
    call:cff euct40
    call:cff eulb40
    call:cff eure40
    call:cff fgv4gm
    call:cff flma40
    call:cff fqef40
    call:cff fqfp40
    call:cff fqin40
    call:cff fqlo40
    call:cff fqst40
    call:cff fsac40
    call:cff fsam40
    call:cff fsef40
    call:cff fsem40
    call:cff fsfl40
    call:cff fsgm40
    call:cff fsia40
    call:cff fspr40
    call:cff fsrd40
    call:cff fssv40
    call:cff hfex40
    call:cff hffo40
    call:cff hfin40
    call:cff hflc40
    call:cff hflo40
    call:cff hflr40
    call:cff hfso40
    call:cff hfsu40
    call:cff inin53
    call:cff lbaj40
    call:cff lbbb40
    call:cff lbci30
    call:cff lbcp40
    call:cff lbcx40
    call:cff lbdc32
    call:cff lbds30
    call:cff lbdy40
    call:cff lbel30
    call:cff lber50
    call:cff lbfi51
    call:cff lbfl51
    call:cff lbfw51diacommandfile
    call:cff lbfw51diadatafile
    call:cff lbfw51diaframe
    call:cff lbfw51FieldValueFile
    call:cff lbfw51filosmodelfile
    call:cff lbfw51Geometry
    call:cff lbfw51guiframe
    call:cff lbfw51midasfxdfile
    call:cff lbfw51ModelFrame
    call:cff lbfw51nastranfile
    call:cff lbfw51Tensor
    call:cff lbfw51TetMesh
    call:cff lbfw51Triangle
    call:cff lbgc51classes
    call:cff lbgc51routines
    call:cff lbge30
    call:cff lbgs30
    call:cff lbio51
    call:cff lbjc51
    call:cff lbma40
    call:cff lbme51
    call:cff lbpd30arpack
    call:cff lbpd30bihar
    call:cff lbpd30ep
    call:cff lbpd30lp
    call:cff lbpd30ls
    call:cff lbpd30metis
    call:cff lbpd30or
    call:cff lbpd30quad
    call:cff lbpd30rkf
    call:cff lbpf40
    call:cff lbpo40
    call:cff lbql30
    call:cff lbrd51
    call:cff lbsc30
    call:cff lbso50
    call:cff lbtm50
    call:cff lbui51
    call:cff lsas40
    call:cff lsdd40
    call:cff lsem40
    call:cff lsev40
    call:cff lsex50
    call:cff lsfu40
    call:cff lsit40
    call:cff lslo40
    call:cff lsrf50
    call:cff mird50
    call:cff moco40
    call:cff mofp40
    call:cff molb40
    call:cff mord40
    call:cff moxq40
    call:cff nlcv41
    call:cff nldu41
    call:cff nlec42
    call:cff nles41
    call:cff nlif41
    call:cff nlin41
    call:cff nlis41
    call:cff nlix41
    call:cff nllb41
    call:cff nllg41
    call:cff nllo41
    call:cff nlpr41
    call:cff nlrd41
    call:cff nlre41
    call:cff nlrf41
    call:cff nlrl41
    call:cff nlst41
    call:cff nltg41
    call:cff nmrs41
    call:cff nmsa41
    call:cff nmst41
    call:cff pein41
    call:cff peka41
    call:cff pelb41
    call:cff pfas40
    call:cff pfbf40
    call:cff pfbo40
    call:cff pfcs40
    call:cff pfem40
    call:cff pfev40
    call:cff pfin40
    call:cff pfmr40
    call:cff pfrn40
    call:cff pfsv40
    call:cff pfut40
    call:cff phin40
    call:cff phsu40
    call:cff piev41
    call:cff pige40
    call:cff pilb41
    call:cff pird40
    call:cff pofp40
    call:cff pofx40
    call:cff pogm40
    call:cff posr40
    call:cff powr42
    call:cff psfs40
    call:cff pssf40
    call:cff sain50
    call:cff samo50
    call:cff sarh50
    call:cff sebo41
    call:cff seio41
    call:cff selb41
    call:cff sest41
    call:cff sete41
    call:cff soso41
    call:cff spxq40
    call:cff utlb01
    call:cff lq

rem -- binseg
    mkdir %1\diana\%REL%\binseg

rem -- binseg/ap
    set SD=%BINSEG%\ap
    set TD=%1\diana\%REL%\binseg\ap
    mkdir %TD%
    call:cf gm42.exe
    call:cf ca40.exe
    call:cf ho40.exe
    call:cf in51.exe
    call:cf mc41.exe
    call:cf se51.exe
    call:cf bo40.exe

rem -- src
    mkdir %1\diana\%REL%\src

rem -- src/include
    set SD=%DIAPATH%\src\include
    set TD=%1\diana\%REL%\src\include
    mkdir %TD%
    call:cf dimple.h
    call:cf f77names.h

    goto:eof

:sharefiles

rem -- share
    mkdir %1\diana\share

    set DIASLIB=%DIASHARE%\lib
    set DIASRC=%DIASHARE%\src

rem -- share/lib
    set SD=%DIASHARE%\lib
    set TD=%1\diana\share\lib
    mkdir %TD%
    call:cf DI_ERROR.DAT
    call:cf femsys.ini
    call:cf filoscap
    call:cf geoclass.res
    call:cf matclass.res

rem -- share/lib/ElmTyp
    mkdir %1\diana\share\lib\ElmTyp
    copy %DIASLIB%\ElmTyp\* %1\diana\share\lib\ElmTyp

rem -- share/src
    mkdir %1\diana\share\src

rem -- share/src/include
    set SD=%DIASRC%\include
    set TD=%1\diana\share\src\include
    mkdir %TD%
    call:cf BoundBox.h
    call:cf d_cpptypes.h
    call:cf DiaAssert.h
    call:cf DiaValueVector.h
    call:cf DianaVersionInfo.h
    call:cf DiMalloc.h
    call:cf fnames.dat
    call:cf fort.h
    call:cf lbci.h
    call:cf lbcp.h
    call:cf lbcx.h
    call:cf lbdc.h
    call:cf lbel.h
    call:cf lbfl.h
    call:cf lbgc.h
    call:cf lbgs.h
    call:cf RBTree.h
    call:cf Set.h
    call:cf Utils.h

rem -- share/src/ap
    mkdir %1\diana\share\src\ap
    copy %DIASRC%\ap\*.txt %1\diana\share\src\ap

rem -- share/src/dc
    mkdir %1\diana\share\src\dc
    copy %DIASRC%\dc\*.txt %1\diana\share\src\dc

rem -- share/src/in
    mkdir %1\diana\share\src\in
    copy %DIASRC%\in\*.txt %1\diana\share\src\in

rem -- share/src/lb
    mkdir %1\diana\share\src\lb
    copy %DIASRC%\lb\*.txt %1\diana\share\src\lb

rem -- share/src/lb/fl51
    set SD=%DIASRC%\lb\fl51
    set TD=%1\diana\share\src\lb\fl51
    mkdir %TD%
    call:cf ds.h
    call:cf filos.h
    call:cf flut.h

rem -- share/src/lb/gc51/classes/include
    mkdir %1\diana\share\src\lb\gc51
    mkdir %1\diana\share\src\lb\gc51\classes
    mkdir %1\diana\share\src\lb\gc51\classes\include
    copy %DIASRC%\lb\gc51\classes\include\*.h %1\diana\share\src\lb\gc51\classes\include

rem -- share/src/ls
    mkdir %1\diana\share\src\ls
    copy %DIASRC%\ls\*.txt %1\diana\share\src\ls

rem -- share/src/nl
    mkdir %1\diana\share\src\nl
    copy %DIASRC%\nl\*.txt %1\diana\share\src\nl

rem -- share/src/pf
    mkdir %1\diana\share\src\pf
    copy %DIASRC%\pf\*.txt %1\diana\share\src\pf

rem -- share/src/po
    mkdir %1\diana\share\src\po
    copy %DIASRC%\po\*.txt %1\diana\share\src\po

rem -- share/src/so
    mkdir %1\diana\share\src\so
    copy %DIASRC%\so\*.txt %1\diana\share\src\so

rem -- share/SafeNet
    xcopy /s/i %SAFENET%\*.* %1\diana\share\SafeNet

rem -- share/Intel
    xcopy /s/i %INTEL%\intel64\*.* %1\diana\share\Intel

    goto:eof

:cf
rem copy file from %SD% to %TD%
    copy "%SD%\%1" "%TD%"
    goto:eof

:cfw
rem copy framework include directory
    mkdir %TD%\%1
    mkdir %TD%\%1\include
    copy %SD%\%1\include\*.h %TD%\%1\include
    goto:eof

:cff
rem copy file from %SD% to %TD%
    copy %SD%\%1.* "%TD%"
    goto:eof

:Usage
rem -- DOS batch scripts escape using ^ character
    echo Usage: %0 ^<diana root dir^> ^<diana share dir^> ^<SafeNet dir^> ^<Intel redist dir^> ^<target directory^>
    echo A directory named 'diana' will be created under ^<target directory^>
    goto End

:End
    endlocal

