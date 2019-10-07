@echo off

set _task=FightingVision2020.exe
set _svr=C:\%_task%

:checkstart
for /f "tokens=5" %%n in ('qprocess.exe ^| find "%_task%" ') do (
    if %%n==%_task% (goto checkag) else goto startsvr
)

:startsvr
echo %time%
echo ********Program Start********
echo Program Start time at %time% ,Please check the log >> restart_service.log
start %_svr%
set/p=.<nul
for /L %%i in (1 1 10) do set /p a=.<nul&ping.exe /n 2 127.0.0.1>nul
echo .
echo Wscript.Sleep WScript.Arguments(0) >%tmp%\delay.vbs
cscript //b //nologo %tmp%\delay.vbs 10000

echo ********Program Startup Completion********
goto checkstart

:checkag
echo %time% The program runs normally and continues to check after 10 seconds...
echo Wscript.Sleep WScript.Arguments(0) >%tmp%\delay.vbs
cscript //b //nologo %tmp%\delay.vbs 10000

goto checkstart