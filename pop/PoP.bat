@echo off
cls
rem Don't want to show too much, do we?
set dir=%~dp0
cd %dir%
rem Working directory, where the batch is located

FOR /F "skip=2 tokens=2*" %%A IN ('REG QUERY "HKLM\Software\JavaSoft\Java Development Kit" /v CurrentVersion') DO set CurVer=%%B
FOR /F "skip=2 tokens=2*" %%A IN ('REG QUERY "HKLM\Software\JavaSoft\Java Runtime Environment" /v CurrentVersion') DO set CurVerJRE=%%B


FOR /F "skip=2 tokens=2*" %%A IN ('REG QUERY "HKLM\Software\JavaSoft\Java Development Kit\%CurVer%" /v JavaHome') DO set javapath=%%B
FOR /F "skip=2 tokens=2*" %%A IN ('REG QUERY "HKLM\Software\JavaSoft\Java Runtime Environment\%CurVerJRE%" /v JavaHome') DO set javapathJRE=%%B
rem The version of Java JDK

rem The next line could be:
rem set path=%path%;%javapath%\bin;%javapathJRE%\bin;
set path=%path%;%javapathJRE%\bin;

rem Run PoP
java -jar PoP.jar