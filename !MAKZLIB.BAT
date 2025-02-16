@echo off
@echo system dos4g > _BUILD\LINK.LNK
@echo build errors > LOGS.LOG
del MAIN.EXE
call _BUILD\CC.BAT ZLIB\ADLER32.C               ADLER32.OBJ
call _BUILD\CC.BAT ZLIB\COMPRESS.C              COMPRESS.OBJ
call _BUILD\CC.BAT ZLIB\CRC32.C                 CRC32.OBJ
call _BUILD\CC.BAT ZLIB\DEFLATE.C               DEFLATE.OBJ
call _BUILD\CC.BAT ZLIB\GZIO.C                  GZIO.OBJ
call _BUILD\CC.BAT ZLIB\INFFAST.C               INFFAST.OBJ
call _BUILD\CC.BAT ZLIB\INFLATE.C               INFLATE.OBJ
call _BUILD\CC.BAT ZLIB\INFTREES.C              INFTREES.OBJ
call _BUILD\CC.BAT ZLIB\TREES.C                 TREES.OBJ
call _BUILD\CC.BAT ZLIB\UNCOMPR.C               UNCOMPR.OBJ
call _BUILD\CC.BAT ZLIB\ZUTIL.C                 ZUTIL.OBJ
me LOGS.LOG