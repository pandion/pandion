@ECHO OFF
SETLOCAL

:: TODO: This information should be centralized and automated. Perhaps using Ant or Maven2?
SET VERSION_MAJOR=2
SET VERSION_MINOR=6

:: Hudson sets its environment variables but defaults are provided for manual builds.
IF NOT DEFINED BUILD_NUMBER SET BUILD_NUMBER=0
IF NOT DEFINED HUDSON_URL SET HUDSON_URL=http://build.pandion.im/
SET PACKAGE=Pandion_%VERSION_MAJOR%.%VERSION_MINOR%.%BUILD_NUMBER%.msi
SET LOCAL_FILE=../WiX/%PACKAGE%

CScript.exe //NoLogo appcaster.wsf /path:index.xml /overwrite:false /application:Pandion /author:"Pandion Team" /description:"Pandion software updates feed" /location:http://feeds.feedburner.com/pandionupdates /history:"%HUDSON_URL%" /arguments:"/passive LAUNCHAPP=yes" /changes:"Development build" /info:"%HUDSON_URL%job/%JOB_NAME%/%BUILD_NUMBER%/changes" /mime:application/x-msi /size:"%LOCAL_FILE%" /track:development /version:"%VERSION_MAJOR%.%VERSION_MINOR%.%BUILD_NUMBER%" /url:"http://appcast.pandion.be/builds/%PACKAGE%"
