@ECHO OFF
SETLOCAL

:: This script creates an MSI package for Windows Installer.
:: Dependency: WiX 3.5 http://wix.sf.net/
:: Note: heat/candle/light need to be accessible in the PATH.
:: Default location:
:: C:\Program Files (x86)\Windows Installer XML v3.5\bin

:: TODO: This information should be centralized and automated. Perhaps using Ant or Maven2?
SET VERSION_MAJOR=2
SET VERSION_MINOR=6

:: Hudson sets its environment variables but defaults are provided for manual builds.
IF NOT DEFINED BUILD_NUMBER SET BUILD_NUMBER=0
SET PACKAGE=Pandion_%VERSION_MAJOR%.%VERSION_MINOR%.%BUILD_NUMBER%.msi

ECHO Creating temporary copy...
:: The XCOPY command is deprecated since Vista and replaced by ROBOCOPY.
:: MKDIR "Temp"
:: XCOPY "../../Client" "./Temp" /E /EXCLUDE:filter.txt
ROBOCOPY "../../Client" "./Temp" /MIR /XF ".gitignore" "pandion.pdb" "pandiond.exe" "pandiond.pdb" /NFL /NDL /NJH

:: Inject the build number into the installed files.
ECHO Build number %BUILD_NUMBER%
ECHO %BUILD_NUMBER% > "./Temp/settings/build.txt"

:: Generate a list of files as a component group.
ECHO Harvesting components...
heat.exe dir "./Temp" -nologo -cg "CoreFiles" -dr "DESTINATION" -srd -template:fragment -ke -gg -out "./components.wxs"

:: Transform the XML.
ECHO Compiling installer...
candle.exe -nologo -sw1077 "./product.wxs" "./components.wxs" "./WixUI_OneClick.wxs" "./AdvancedWelcomeEulaDlg_OneClick.wxs"

:: Create the MSI distributable.
ECHO Linking installer...
light.exe -nologo -b "./Temp" -ext "WixUIExtension" -ext "WixUtilExtension" -cultures:en-us -sice:ICE38 -sice:ICE64 -sice:ICE91 -out "./%PACKAGE%" "./product.wixobj" "./components.wixobj" "./WixUI_OneClick.wixobj" "./AdvancedWelcomeEulaDlg_OneClick.wixobj"

ECHO Cleaning up...
:: RMDIR /S /Q "./Temp"
DEL "./components.wxs" "./*.wixobj" "./*.wixpdb" /Q

ECHO Done: %PACKAGE%
