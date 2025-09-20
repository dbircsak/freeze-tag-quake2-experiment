@echo off
set Q2DIR=C:\Quake2

copy /Y "gamex86.dll" "%Q2DIR%\freeze\"
copy /Y "autoexec.cfg" "%Q2DIR%\freeze\"
copy /Y "freeze.cfg" "%Q2DIR%\freeze\"
copy /Y "freeze.ini" "%Q2DIR%\freeze\"
copy /Y "pak2.pak" "%Q2DIR%\freeze\"
start "" /D "%Q2DIR%" quake2.exe +set game freeze +exec freeze.cfg +map strike +set dedicated 1 +set developer 1
