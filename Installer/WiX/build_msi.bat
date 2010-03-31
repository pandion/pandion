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
:: The XCOPY command is deprecated since Vista and replaced by ROBOCOPY.
:: MKDIR "Temp"
:: XCOPY "../../Client" "./Temp" /E /EXCLUDE:filter.txt
ROBOCOPY "../../Client" "./Temp" /MIR /XF ".gitignore" "pandion.pdb" "pandiond.exe" "pandiond.pdb" /NFL /NDL /NJH
IF ERRORLEVEL 8 ECHO Error: Cannot create temporary copy && EXIT /B 1

:: Inject the build number into the installed files.
ECHO Build number %BUILD_NUMBER%
ECHO %BUILD_NUMBER% > "./Temp/settings/build.txt"

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
