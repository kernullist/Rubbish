@echo off

SET mypath=%~dp0

call sc create KbdRdr binPath= "%mypath%KbdRdr.sys" type= kernel displayName= KbdRdr
if %ERRORLEVEL% NEQ 0 goto Error
call sc start KbdRdr
if %ERRORLEVEL% NEQ 0 goto Error

:Error
echo Exit Code : %ERRORLEVEL%

pause