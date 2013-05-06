@echo off
setlocal
@echo off

REM # Get the location of quickbuild.bat and drop trailing slash
set ROOT=%~dp0
set ROOT=%ROOT:~0,-1%
set BOOST_BUILD_PATH=%ROOT%\freicore\libraries\boost-build

set BJAM=%BOOST_BUILD_PATH%\engine\bin.ntx86\bjam.exe

REM # msvc.jam assumes it will find "ShowVer.exe" in %PATH%
set PATH=%PATH%;%ROOT%\freicore\libraries

REM # Build local copy of bjam
IF EXIST %BJAM% GOTO SKIP_BJAM
echo Building bjam...
pushd %BOOST_BUILD_PATH%\engine
call build.bat
@echo off
setlocal
@echo off
popd
:SKIP_BJAM

REM # Do full build, passing quickbuild's arguments to bjam
pushd %ROOT%
%BJAM% %*
popd
