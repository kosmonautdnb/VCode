@echo off
@echo Starting..
@echo system dos4g > _BUILD\LINK.LNK
@echo build errors > LOGS.LOG
@echo Deleting exe..
FLTMECHK.exe _BUILD\MAKEFILE MAIN.exe
if not ERRORLEVEL 255 call _BUILD\MAKEMK.BAT
del MAIN.EXE
call wmake.exe /f _build\makefile /e /s /k
m logs.log