cat ../dialibsenv.bat | grep 'set DLL' | sed 's/^.*set //' | sed 's/\.dll//' > diana.pri
cat ../dialibsenv.bat | grep 'set LIB' | sed 's/^.*set //' | sed 's/\.lib//' >> diana.pri
