@echo off
setlocal

call "%VS100COMNTOOLS%vsvars32.bat"

mode 160,20

echo.
echo ~~~~~~ buildsdkshaders %* ~~~~~~
echo.

set GAMEDIR="F:\source-sdk-2013\sp\game\mod_hl2"

set SDKBINDIR="E:\Program Files (x86)\Steam\steamapps\common\Source SDK Base 2013 Singleplayer\bin"

set BUILD_SHADER=call build_shaders.bat

echo mod directory: %GAMEDIR%
echo sdk bin directory: %SDKBINDIR%

%BUILD_SHADER% postprocess_shaders -dx9_30 -force30

echo Finished compiling shaders.
pause