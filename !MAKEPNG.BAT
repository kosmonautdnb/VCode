@echo off
@echo system dos4g > _BUILD\LINK.LNK
@echo build errors > LOGS.LOG
del MAIN.EXE
call _BUILD\CC.BAT PNG\PNG.C                    PNG.OBJ
call _BUILD\CC.BAT PNG\PNGERROR.C               PNGERROR.OBJ
call _BUILD\CC.BAT PNG\PNGGET.C                 PNGGET.OBJ
call _BUILD\CC.BAT PNG\PNGMEM.C                 PNGMEM.OBJ
call _BUILD\CC.BAT PNG\PNGPREAD.C               PNGPREAD.OBJ
call _BUILD\CC.BAT PNG\PNGREAD.C                PNGREAD.OBJ
call _BUILD\CC.BAT PNG\PNGRIO.C                 PNGPRIO.OBJ
call _BUILD\CC.BAT PNG\PNGRTRAN.C               PNGRTRAN.OBJ
call _BUILD\CC.BAT PNG\PNGRUTIL.C               PNGRUTIL.OBJ
call _BUILD\CC.BAT PNG\PNGSET.C                 PNGSET.OBJ
call _BUILD\CC.BAT PNG\PNGTRANS.C               PNGTRANS.OBJ
call _BUILD\CC.BAT PNG\PNGWIO.C                 PNGWIO.OBJ
call _BUILD\CC.BAT PNG\PNGWRITE.C               PNGWRITE.OBJ
call _BUILD\CC.BAT PNG\PNGWTRAN.C               PNGWTRAN.OBJ
call _BUILD\CC.BAT PNG\PNGWUTIL.C               PNGWUTIL.OBJ
me LOGS.LOG