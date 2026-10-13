@echo off

cd ../build

echo ======================================== >> engineLogs.txt
echo [Session Started: %DATE% %TIME%] >> engineLogs.txt
echo ======================================== >> engineLogs.txt

Francum.exe >> engineLogs.txt 2>&1

echo. >> engineLogs.txt
echo [!!! ENGINE CRASHED OR EXITED AT %TIME% !!!] >> engineLogs.txt
echo. >> engineLogs.txt

exit