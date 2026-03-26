@echo off
PATH .;\;\LOCALE;\_DOS_\BIN;\_DOS_\OWN;C:\_WAT_C_\BINW;C:\DJGPP\BIN;C:\grafx2
display con=(ega,,1)
SET DOSDIR=C:\_DOS_
SET INCLUDE=C:\_WAT_C_\H
SET WATCOM=C:\_WAT_C_
SET EDPATH=C:\_WAT_C_\EDDAT
SET DJGPP=C:\DJGPP\DJGPP.ENV
CTMOUSE /R11
cd c:\_OPENGL_
doslfn
LH C:\_DOS_\OWN\RDISK.COM /:R /S32
SETLANG DE

rem GOTO %CONFIG%
rem :1
rem mode con codepage prepare=((858) \locale\ega.cpx) > NUL
rem mode con codepage select=858 > NUL
rem keyb gr,,\locale\keyboard.sys
rem :2