@echo off
setlocal
@echo off

REM # Get the location of quickbuild.bat and drop trailing slash
set ROOT=%~dp0
set ROOT=%ROOT:~0,-1%
pushd %ROOT%

CALL freicore/clean.bat

IF EXIST build-nt-x86 rmdir /s /q build-nt-x86
IF EXIST build-nt-x86_64 rmdir /s /q build-nt-x86_64

popd