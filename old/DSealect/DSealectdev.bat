    @echo off

rem -- Keep environment variables local to this batch file
    setlocal

    call DSealectsetenv

    set AST=
    for /f "delims=" %%i in ('svn status -qu %FWDIR% ^| findstr "*"') do @set AST=%%i
    if NOT "%AST%"=="" (
      echo ******
      echo ** There is an update available for %FWDIR%
      echo ******
      echo.
    )

    set DEVENV="C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe"
    set SOLUTION="%DEVELOP%\DSealect\DSealect.sln"

    set CMD=%DEVENV% %SOLUTION%

    echo Starting command %CMD%
    %CMD%

    endlocal
