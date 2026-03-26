@echo .AS "%1" : "R:\%2"
wasm.exe -5pr -fp5 -zq %1 -fR:\%2 >> R:\LOGS.LOG
@echo file R:\%2 >> R:\LINK.LNK