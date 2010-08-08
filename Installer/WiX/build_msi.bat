:: This file is part of Pandion instant messenger
:: Copyright (c) 2009-2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
IF NOT DEFINED BUILD_CONFIG CALL "..\..\build_config.bat"

:: This script creates an MSI package for Windows Installer.
:: Dependency: WiX 3.5 http://wix.sf.net/
:: Note: heat/candle/light need to be accessible in the PATH.
:: Default location:
:: C:\Program Files (x86)\Windows Installer XML v3.5\bin

ECHO Creating temporary copy...
ROBOCOPY "../../Client" "./Temp" /MIR /XF ".gitignore" "*.pdb" "*_debug.exe" /E /NJH /NJS /NS /NC /NFL /NDL
IF %ERRORLEVEL% GEQ 8 ECHO Error: Cannot create temporary copy && EXIT /B 1
IF EXIST ".\Temp\%JOB_NAME_SAFE%.exe" DEL /F /Q ".\Temp\%JOB_NAME_SAFE%.exe"
IF EXIST ".\Temp\Host.exe" RENAME ".\Temp\Host.exe" "%JOB_NAME_SAFE%.exe"
IF NOT EXIST ".\Temp\%JOB_NAME_SAFE%.exe" ECHO Error: Cannot include Host.exe && EXIT /B 1

:: Inject the build number into the installed files.
ECHO Build number %VERSION_BUILD%
ECHO %VERSION_BUILD% > "./Temp/settings/build.txt"

:: Generate a list of files as a component group.
ECHO Harvesting components...
heat.exe dir "./Temp" -nologo -cg "CoreFiles" -dr "DESTINATION" -srd -template fragment -ke -gg -out "./components.wxs"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to harvest components && EXIT /B 1

:: Transform the XML.
ECHO Compiling installer...
candle.exe -nologo -sw1077 "./product.wxs" "./components.wxs" "./WixUI_ZeroClick.wxs"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to compile installer && EXIT /B 1

:: Create the MSI distributable.
ECHO Linking installer...
light.exe -nologo -b "./Temp" -ext "WixUIExtension" -ext "WixUtilExtension" -cultures:en-us -sice:ICE38 -sice:ICE64 -sice:ICE91 -out "./%PACKAGE%" "./product.wixobj" "./components.wixobj" "./WixUI_ZeroClick.wixobj"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to link installer && EXIT /B 1

ECHO Cleaning up...
:: RMDIR /S /Q "./Temp"
DEL "./components.wxs" "./*.wixobj" "./*.wixpdb" /Q

ECHO Done: %PACKAGE%
