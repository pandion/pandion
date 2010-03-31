:: This file is part of Pandion instant messenger
:: Copyright (c) 2009-2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
IF NOT DEFINED BUILD_CONFIG CALL "..\build_config.bat"

ECHO #define FILEVER %VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0 > ".\Source\VersionNo.h"
ECHO #define PRODUCTVER %VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0 >> ".\Source\VersionNo.h"
ECHO #define FILEVERSTR "%VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0\0" >> ".\Source\VersionNo.h"
ECHO #define PRODUCTVERSTR "%VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0\0" >> ".\Source\VersionNo.h"

CALL "%VS90COMNTOOLS%\vsvars32.bat"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Cannot set environment build variables && EXIT /B 1

VCBUILD /useenv /rebuild "host.sln" "Release|Win32"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to compile Host && EXIT /B 1
