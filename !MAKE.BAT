@echo off
@echo Starting.. (needs RamDisk at R:)
@echo system dos4g > R:\LINK.LNK
rem @echo system pmodew > R:\LINK.LNK
@echo build errors > R:\LOGS.LOG
_BUILD\FLTMECHK.exe !MAK R:\gltest.exe
if not ERRORLEVEL 255 call _BUILD\MAKEMK.BAT
call wmake.exe /f R:\makefile /e /s /k