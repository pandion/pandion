@ECHO OFF
SETLOCAL

:: This script creates an MSI package for Windows Installer.
:: Dependency: WiX 3.5 http://wix.sf.net/
:: Note: heat/candle/light need to be accessible in the PATH
:: Default location:
:: C:\Program Files (x86)\Windows Installer XML v3.5\bin

ECHO Creating temporary copy...
:: The XCOPY command is deprecated since Vista and replaced by ROBOCOPY
:: MKDIR "Temp"
:: XCOPY "../../Client" "./Temp" /E /EXCLUDE:filter.txt
ROBOCOPY "../../Client" "./Temp" /MIR /XF ".gitignore" "pandion.pdb" "pandiond.exe" "pandiond.pdb" /NFL /NDL /NJH

:: Generate a list of files as a component group.
ECHO Harvesting components...
heat.exe dir "./Temp" -nologo -cg "CoreFiles" -dr "DESTINATION" -srd -template:fragment -ke -gg -out "./components.wxs"

:: Transform the XML
ECHO Compiling installer...
candle.exe -nologo -sw1077 "./product.wxs" "./components.wxs" "./WixUI_OneClick.wxs" "./AdvancedWelcomeEulaDlg_OneClick.wxs"

:: Create the MSI distributable
ECHO Linking installer...
light.exe -nologo -b "./Temp" -ext "WixUIExtension" -ext "WixUtilExtension" -cultures:en-us -sice:ICE38 -sice:ICE64 -sice:ICE91 -out "./Pandion.msi" "./product.wixobj" "./components.wixobj" "./WixUI_OneClick.wixobj" "./AdvancedWelcomeEulaDlg_OneClick.wixobj"

ECHO Cleaning up...
:: RMDIR /S /Q "./Temp"
DEL "./components.wxs" "./*.wixobj" "./*.wixpdb" /Q

ECHO Done: Pandion.msi
