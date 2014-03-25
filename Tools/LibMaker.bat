@echo off
setlocal enableextensions EnableDelayedExpansion

set LibValues=lldiv;lldvrm;llmul;llrem;llshl;llshr;ulldiv;ulldvrm;ullrem;ullshr;chkstk

if "%2" == "" goto Usage

echo.
echo Step 1: Searching for %1

for %%a in ("%libpath:;=";"%") do (
	set FilePath=%%a\%1
	pushd %%a
	if exist %1 ( 
		popd
		goto Process
		echo !FilePath!
	) 
	popd
)

echo %1 Not found

goto End

:Process

echo Step 2: Proessing %1

lib /nologo /list:LibOut.txt !FilePath! 

rem for /F %%e in (LibOut.txt) do echo %%e

echo. > LibPass.txt

for %%a in ("%LibValues:;=";"%") do (
	Findstr \\%%~a.obj LibOut.txt >> LibPass.txt
)
rem Findstr mt_obj\\asm\\ LibOut.txt > LibPass.txt

echo Step 3: Extracting Files
del /q *.obj 2> nul

for /F %%o in (LibPass.txt) do (
lib /nologo /extract:%%o %1
)

echo Step 4: Build %2

lib /nologo  /out:%2 *.obj

echo Step 5: Clean up
del /q *.obj 2> nul


goto End

:Usage

echo.
echo LibMaker SourceLib OutputLib 
echo.
echo Libmaker will extract a know set of obj files from the source lib
echo And use them to build the output lib.
echo.
echo All know Library paths will be searched based on the LIBPATH variable
echo.

:End
endlocal