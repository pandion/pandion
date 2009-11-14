@ECHO OFF
SETLOCAL

:: TODO: This information should be centralized and automated. Perhaps using Ant or Maven2?
SET VERSION_MAJOR=2
SET VERSION_MINOR=6

:: Hudson sets its environment variables but defaults are provided for manual builds.
IF NOT DEFINED BUILD_NUMBER SET BUILD_NUMBER=0
IF NOT DEFINED HUDSON_URL SET HUDSON_URL=http://build.pandion.im/
IF NOT DEFINED JOB_NAME SET JOB_NAME=Pandion
SET VERSION=%VERSION_MAJOR%.%VERSION_MINOR%.%BUILD_NUMBER%
SET PACKAGE=%JOB_NAME%_%VERSION%.msi
SET LOCAL_FILE=../WiX/%PACKAGE%
SET DOWNLOAD_URL=http://downloads.sourceforge.net/project/pandion/builds/%PACKAGE%
SET INFO_URL=%HUDSON_URL%job/%JOB_NAME%/%BUILD_NUMBER%/changes
SET FEED_URL=http://feeds.feedburner.com/pandionupdates

CScript.exe //NoLogo appcaster.wsf /maxcount:25 /source:"%FEED_URL%" /path:index.xml /overwrite:false /application:"%JOB_NAME%" /author:"%JOB_NAME% Team" /description:"%JOB_NAME% software updates feed" /location:"%FEED_URL%" /history:"%HUDSON_URL%" /arguments:"/passive LAUNCHAPP=yes AUTOUPDATE=yes" /changes:"Development build" /info:"%INFO_URL%" /mime:application/x-msi /size:"%LOCAL_FILE%" /track:development /version:"%VERSION%" /url:"%DOWNLOAD_URL%"
