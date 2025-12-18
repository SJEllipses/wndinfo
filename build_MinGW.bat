@echo off

pushd %~dp0

echo Compiling wndinfo...

windres .\resource.rc -O coff -o .\resource.o
g++ .\wndinfo.cpp .\resource.o -o .\wndinfo.exe -mwindows -lgdi32 -ldwmapi -s

rm .\resource.o

popd

echo.
echo Build finished successfully.