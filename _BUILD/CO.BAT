@echo off
@echo .CO: "R:\%2"
rem wcc386.exe -5r -fp5 -d3 -d+ -mc -zpw -zq -zp=1 %1 -foR:\%2 >> R:\LOGS.LOG
rem wcl386.exe /c /5r /fp5 /d3 /d+ /mf /zpw /zq /zp=1 %1 -foR:\%2 >> R:\LOGS.LOG
@echo file R:\%2 >> R:\LINK.LNK