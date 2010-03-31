:: This file is part of Pandion instant messenger
:: Copyright (c) 2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
IF NOT DEFINED BUILD_CONFIG CALL "build_config.bat"

:: Compile Host
ECHO Compiling Host...
CD ".\Host"
CALL "build_host.bat"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Cannot compile Host && EXIT /B 1
CD ..

:: Build MSI package
ECHO Building MSI package...
CD ".\Installer\WiX"
DEL "*.msi" /Q
CALL "build_msi.bat"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Cannot build MSI package && EXIT /B 1
CD "..\.."

:: Generating appcasting feed
ECHO Generate appcasting feed...
CD ".\Installer\Appcaster"
CALL "build_feed.bat"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Cannot generate appcasting feed && EXIT /B 1
CD "..\.."
