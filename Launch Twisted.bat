@echo off
rem Launches Quake 4 with the Twisted mod.
rem Works when placed in the Quake 4 folder, or from anywhere on a default Steam install.

set "Q4DIR=%~dp0"
if exist "%Q4DIR%Quake4.exe" goto run

set "Q4DIR=C:\Program Files (x86)\Steam\steamapps\common\Quake 4\"
if exist "%Q4DIR%Quake4.exe" goto run

echo Could not find Quake4.exe.
echo Copy this file into your Quake 4 folder (next to Quake4.exe) and run it again.
pause
exit /b

:run
cd /d "%Q4DIR%"
start "" /D "%Q4DIR%" "%Q4DIR%Quake4.exe" +set fs_game Twisted