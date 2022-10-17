@echo off

echo Compiling the WF121 project...
C:\Bluegiga\wf121-1.4.1-91\bin\bgbuild.exe .\project.xml

echo Correcting the MAC address...
:: Change MAC address to Bluegiga (00:07:80) + Carnegie Space (c5) + Iris (12:15)
C:\Bluegiga\wf121-1.4.1-91\bin\changemac.exe .\build\iris_wifi_radio__auto_connect.hex 00:07:80:c5:12:15

echo Cleaning Up...
move .\variable_memory_usage.txt .\build

pause