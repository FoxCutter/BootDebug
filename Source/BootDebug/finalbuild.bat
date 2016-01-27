@echo off

set ToolFolder=%~dp1..\..\Tools
set ImagePath=D:\My Virtual Machines\VirtualBox VMs\Grub\Grub.vhd
set OutputPath=(hd1)\Kernal.exe

%ToolFolder%\HeaderSet %1

rem echo %ERRORLEVEL%
IF ERRORLEVEL 0 goto DiskTools

echo HearderSet : Error: can't update header
goto end

:DiskTools
%ToolFolder%\DiskTools add "%ImagePath%" "%1" "%OutputPath%"
IF ERRORLEVEL 0 goto end

echo DiskTools : Error: can't update image
goto end


:end

