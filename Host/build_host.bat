:: This file is part of Pandion instant messenger
:: Copyright (c) 2009-2010 Sebastiaan Deckers
:: License: GNU General Public License version 3 or later

@ECHO OFF
IF NOT DEFINED BUILD_CONFIG CALL "..\build_config.bat"

:: Prepare Visual C++ 2010
IF NOT DEFINED VS100COMNTOOLS ECHO Error: Microsoft Visual C++ 2010 (Express or Professional) is required. && EXIT /B 1
CALL "%VS100COMNTOOLS%\vsvars32.bat"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Cannot set environment build variables. && EXIT /B 1

:: Set meta information
ECHO #define FILEVER %VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0 > ".\Source\VersionNo.h"
ECHO #define PRODUCTVER %VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0 >> ".\Source\VersionNo.h"
ECHO #define FILEVERSTR "%VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0\0" >> ".\Source\VersionNo.h"
ECHO #define PRODUCTVERSTR "%VERSION_MAJOR%,%VERSION_MINOR%,%BUILD_NUMBER%,0\0" >> ".\Source\VersionNo.h"

ECHO #define COMMENTS "%HOST_COMMENTS%" >> ".\Source\VersionNo.h"
ECHO #define COMPANY "%HOST_COMPANY%" >> ".\Source\VersionNo.h"
ECHO #define DESCRIPTION "%HOST_DESCRIPTION%" >> ".\Source\VersionNo.h"
ECHO #define COPYRIGHT "%HOST_COPYRIGHT%" >> ".\Source\VersionNo.h"

:: Start the build
MSBUILD /p:useenv=true /t:rebuild /p:Configuration=Release /p:Platform=Win32 "host.sln"
IF %ERRORLEVEL% NEQ 0 ECHO Error: Failed to compile Host && EXIT /B 1
