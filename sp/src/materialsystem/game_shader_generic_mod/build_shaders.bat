@echo off

set TTEXE=time /t

echo.
rem echo ==================== buildshaders %* ==================
%TTEXE% -cur-Q
set tt_start=%ERRORLEVEL%
set tt_chkpt=%tt_start%

REM ****************
REM usage: buildshaders <shaderProjectName>
REM ****************

setlocal
set arg_filename=%1
set shadercompilecommand=shadercompile.exe
set targetdir=shaders
set SrcDirBase=..\..
set shaderDir=shaders
set SDKArgs=-nompi -nop4 -game %GAMEDIR%
set SHADERINCPATH=vshtmp9/... fxctmp9/...

set inputbase=%1

set DIRECTX_SDK_VER=pc09.00
set DIRECTX_SDK_BIN_DIR=dx9sdk\utilities

if /i "%2" == "-dx9_30" goto dx_sdk_dx9_30
goto dx_sdk_end
:dx_sdk_dx9_30
			set DIRECTX_SDK_VER=pc09.30
			set DIRECTX_SDK_BIN_DIR=dx10sdk\utilities\dx9_30
			goto dx_sdk_end
:dx_sdk_end

if /i "%3" == "-force30" goto set_force30_arg
goto set_force_end
:set_force30_arg
			set DIRECTX_FORCE_MODEL=30
			goto set_force_end
:set_force_end

set targetdir=%GAMEDIR%\shaders

rem if not exist "%GAMEDIR%\gameinfo.txt" goto InvalidGameDirectory
goto build_shaders

REM ****************
REM ERRORS
REM ****************
:InvalidGameDirectory
echo -
echo Error: "%GAMEDIR%" is not a valid game directory.
echo (The -game directory must have a gameinfo.txt file)
echo -
goto end


REM ****************
REM BUILD SHADERS
REM ****************
:build_shaders

rem echo --------------------------------
rem echo %inputbase%
rem echo --------------------------------
REM make sure that target dirs exist
REM files will be built in these targets and copied to their final destination
if not exist %shaderDir% mkdir %shaderDir%
if not exist %shaderDir%\fxc mkdir %shaderDir%\fxc
if not exist %shaderDir%\vsh mkdir %shaderDir%\vsh
if not exist %shaderDir%\psh mkdir %shaderDir%\psh
REM Nuke some files that we will add to later.
if exist filelist.txt del /f /q filelist.txt
if exist filestocopy.txt del /f /q filestocopy.txt
if exist filelistgen.txt del /f /q filelistgen.txt
if exist inclist.txt del /f /q inclist.txt
if exist vcslist.txt del /f /q vcslist.txt

REM ****************
REM Generate a makefile for the shader project
REM ****************
perl "%SrcDirBase%\devtools\bin\updateshaders.pl" -source "%SrcDirBase%" %inputbase%


REM ****************
REM Run the makefile, generating minimal work/build list for fxc files, go ahead and compile vsh and psh files.
REM ****************
rem nmake /S /C -f makefile.%inputbase% clean > clean.txt 2>&1
echo Building inc files, asm vcs files, and VMPI worklist for %inputbase%...
nmake /S /C -f makefile.%inputbase%

REM ****************
REM Copy the inc files to their target
REM ****************
if exist "inclist.txt" (
	echo Publishing shader inc files to target...
	perl %SrcDirBase%\devtools\bin\copyshaderincfiles.pl inclist.txt
)

REM ****************
REM Add the executables to the worklist.
REM ****************
if /i "%DIRECTX_SDK_VER%" == "pc09.00" (
	rem echo "Copy extra files for dx 9 std
)
if /i "%DIRECTX_SDK_VER%" == "pc09.30" (
	echo %SrcDirBase%\devtools\bin\d3dx9_33.dll >> filestocopy.txt
)

echo %SrcDirBase%\%DIRECTX_SDK_BIN_DIR%\dx_proxy.dll >> filestocopy.txt

if not exist %SrcDirBase%\devtools\bin\shadercompile.exe XCOPY %SDKBINDIR%\shadercompile.exe %SrcDirBase%\devtools\bin /D /y
if not exist %SrcDirBase%\devtools\bin\shadercompile_dll.dll XCOPY %SDKBINDIR%\shadercompile_dll.dll %SrcDirBase%\devtools\bin /D /y
if not exist %SrcDirBase%\devtools\bin\vstdlib.dll XCOPY %SDKBINDIR%\vstdlib.dll %SrcDirBase%\devtools\bin /D /y
if not exist %SrcDirBase%\devtools\bin\tier0.dll XCOPY %SDKBINDIR%\tier0.dll %SrcDirBase%\devtools\bin /D /y

echo %SrcDirBase%\devtools\bin\shadercompile.exe >> filestocopy.txt
echo %SrcDirBase%\devtools\bin\shadercompile_dll.dll >> filestocopy.txt
echo %SrcDirBase%\devtools\bin\vstdlib.dll >> filestocopy.txt
echo %SrcDirBase%\devtools\bin\tier0.dll >> filestocopy.txt

REM ****************
REM Cull duplicate entries in work/build list
REM ****************
if exist filestocopy.txt type filestocopy.txt | perl "%SrcDirBase%\devtools\bin\uniqifylist.pl" > uniquefilestocopy.txt
if exist filelistgen.txt if not "%dynamic_shaders%" == "1" (
    echo Generating action list...
    copy filelistgen.txt filelist.txt >nul
)

REM ****************
REM Execute distributed process on work/build list
REM ****************

set shader_path_cd=%cd%
if exist "filelist.txt" if exist "uniquefilestocopy.txt" if not "%dynamic_shaders%" == "1" (
	echo Running distributed shader compilation...

	cd /D %SDKBINDIR%
	%shadercompilecommand% %SDKArgs% -shaderpath "%shader_path_cd:/=\%" -allowdebug
	cd /D %shader_path_cd%
)

REM ****************
REM PC Shader copy
REM Publish the generated files to the output dir using XCOPY
REM This batch file may have been invoked standalone or slaved (master does final smart mirror copy)
REM ****************
:DoXCopy
if not "%dynamic_shaders%" == "1" (
if not exist "%targetdir%" md "%targetdir%"
if not "%targetdir%"=="%shaderDir%" xcopy %shaderDir%\*.* "%targetdir%" /e /y
)
goto end

REM ****************
REM END
REM ****************
:end


%TTEXE% -diff %tt_start%
echo.