setlocal

set PATH=%PATHPREFIX%;.;%PATH%

.\%2 --gm-model-path=%1

endlocal
