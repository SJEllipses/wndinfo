@echo off

pushd %~dp0

echo Compiling wndinfo...

rc .\resource.rc
cl /nologo /utf-8 /O2 /MD /DNDEBUG .\wndinfo.cpp .\resource.res /Fe:wndinfo.exe /link /OPT:REF /OPT:ICF gdi32.lib dwmapi.lib user32.lib shell32.lib advapi32.lib /SUBSYSTEM:WINDOWS

rm .\resource.res
rm .\wndinfo.obj

popd

echo.
echo Build finished successfully.