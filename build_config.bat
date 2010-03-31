:: This file is part of Pandion instant messenger
:: Copyright (c) 2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
SET BUILD_CONFIG=1

:: Versioning settings
SET VERSION_MAJOR=2
SET VERSION_MINOR=6

:: Hudson sets its environment variables but defaults are provided for manual builds.
IF NOT DEFINED BUILD_NUMBER SET BUILD_NUMBER=0
IF NOT DEFINED HUDSON_URL SET HUDSON_URL=http://build.pandion.im/
IF NOT DEFINED JOB_NAME SET JOB_NAME=Pandion

:: WiX settings
SET VERSION=%VERSION_MAJOR%.%VERSION_MINOR%.%BUILD_NUMBER%
SET PACKAGE=%JOB_NAME%_%VERSION%.msi

:: Appcast settings
SET APPCAST_LOCAL_FILE=../WiX/%PACKAGE%
SET APPCAST_DOWNLOAD_URL=http://downloads.sourceforge.net/project/pandion/builds/%PACKAGE%
SET APPCAST_INFO_URL=%HUDSON_URL%job/%JOB_NAME%/%BUILD_NUMBER%/changes
SET APPCAST_FEED_URL=http://feeds.feedburner.com/pandionupdates
