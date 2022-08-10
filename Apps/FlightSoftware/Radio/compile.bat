@echo off

echo Compiling the WF121 project...
C:\Bluegiga\wf121-1.4.1-91\bin\bgbuild.exe .\src\project.xml

echo Correcting the MAC address...
:: Change MAC address to Bluegiga (00:07:80) + Iris (12:15) + IF (ff)
C:\Bluegiga\wf121-1.4.1-91\bin\changemac.exe .\build\iris_wifi_radio__auto_connect.hex 00:07:80:12:15:ff

pause