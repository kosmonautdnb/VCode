@echo off
echo .AL "%1" : "%2"
wlink @_build\pmodew.lnk @%1 OPTION QUIET OPTION STack = 65536 NAME %2 >> R:\LOGS.LOG
rem /Axxxx means how many 4mb pages to reserve seems 256mb (64) is max (in pwmsetup.c)
rem /Vxxxx also means how many 4mb pages to reserve seems 256mb (64) is max (in pwmsetup.c)
rem /Pxxxx protected mode stack size a 16 bytes (a paragraph is 16 bytes) seems the limit is 0400 or 1024*16 so to say 16k (in pwmsetup.c)
_build\pmwsetup.exe /M0 /B0 /A64 /V64 /P1024 %2 > NUL
touch _build\al.bat