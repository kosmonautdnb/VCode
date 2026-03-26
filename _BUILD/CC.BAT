@echo off
@echo .CC: "%1" : "R:\%2"
rem wcc386.exe -5r -fp5 -d3 -d+ -mc -zpw -zq -zp=1 %1 -foR:\%2 >> R:\LOGS.LOG
set DOS4G=quiet
wcl386.exe /c /5r /xr /fp3 /fp5 /ot /on /ol /or /om /mf /zpw /zq /zp1 %1 -foR:\%2 >> R:\LOGS.LOG
rem @echo file R:\%2 >> R:\LINK.LNK