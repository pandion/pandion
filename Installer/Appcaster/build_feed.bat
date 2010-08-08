:: This file is part of Pandion instant messenger
:: Copyright (c) 2009-2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
IF NOT DEFINED BUILD_CONFIG CALL "..\..\build_config.bat"

CScript.exe appcaster.wsf ^
	//NoLogo ^
	/maxcount:3 ^
	/source:"%APPCAST_FEED_URL%" ^
	/path:index.xml ^
	/overwrite:false ^
	/application:"%JOB_NAME%" ^
	/author:"%JOB_NAME% Team" ^
	/description:"%JOB_NAME% software updates feed" ^
	/location:"%APPCAST_FEED_URL%" ^
	/history:"%HUDSON_URL%" ^
	/arguments:"/passive LAUNCHAPP=yes" ^
	/changes:"%APPCAST_TRACK% track" ^
	/info:"%APPCAST_INFO_URL%" ^
	/mime:application/x-msi ^
	/size:"%APPCAST_LOCAL_FILE%" ^
	/track:%APPCAST_TRACK% ^
	/version:"%VERSION%" ^
	/url:"%APPCAST_DOWNLOAD_URL%"

IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to generate appcasting feed && EXIT /B 1
