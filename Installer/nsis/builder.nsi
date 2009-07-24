; This file is part of Pandion instant messenger
; Copyright (c) 2001-2009 Sebastiaan Deckers
; License: GNU General Public License version 3 or later

;--------------------------------
;Basic Settings

!include .\nsis\version.nsi
InstallDir "$PROGRAMFILES\${PRODUCTSAFE}"

;The base URL where to download various components or comment out to bundle all dependencies
!define ONLINE_INSTALL "http://www.pandion.be/download"

;--------------------------------
;Configuration

SetCompressor /SOLID /FINAL lzma
!packhdr "temp.temp.temp" '"${NSISDIR}\upx.exe" -9 temp.temp.temp'

Name "${PRODUCT}"

!include "MUI.nsh"
!include "ZipDLL.nsh"

!define MUI_ICON   "${SOURCEDIR}\images\brand\installer.ico"
!define MUI_UNICON "${SOURCEDIR}\images\brand\installer.ico"
!define MUI_ABORTWARNING
!define MUI_COMPONENTSPAGE_SMALLDESC
!define MUI_FINISHPAGE_RUN "$INSTDIR\${PRODUCTSAFE}.exe"
;!define MUI_FINISHPAGE_NOAUTOCLOSE

;!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${SOURCEDIR}\License.rtf"
;!insertmacro MUI_PAGE_COMPONENTS
;!insertmacro MUI_PAGE_DIRECTORY

!define MUI_PAGE_CUSTOMFUNCTION_LEAVE hideWindow
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

OutFile "${PRODUCTSAFE}-${VERSION}.exe"

ShowInstDetails nevershow
ShowUninstDetails nevershow
CRCCheck on
XPStyle on

!include .\languages\translations.nsi
!include .\lib\utils.nsi
!include .\nsis\sections.nsi
!include .\nsis\uninstall.nsi

;--------------------------------
;Pre-install checks

Function .onInit

  ;--------------------------------
  ;Make sure the user has administrative access
  Call IsUserAdmin
  Pop $R0   ; at this point $R0 is "true" or "false"
  StrCmp $R0 "false" 0 lbl_admin_access
    SetSilent normal
;    MessageBox MB_OK|MB_ICONEXCLAMATION $(TEXT_AdminRequired) /SD IDOK
    MessageBox MB_OK|MB_ICONEXCLAMATION "Error: Cannot install ${PRODUCT} ${VERSION}.$\n$\nLog in to Windows with administrative access and try again, or contact your IT administrator to install ${PRODUCT}" /SD IDOK
    Abort
  lbl_admin_access:

  ;--------------------------------
  ;Close previous instances
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCTSAFE}" "UninstallString"
  IfErrors lbl_install_skip_closerunning
  Push $R0
  Push "\"
  Call ChopAtLastIndexOf
  Pop $R0
  StrCmp $R0 "" lbl_install_skip_closerunning
  StrCpy $R0 $R0 "" 1
  StrCpy $INSTDIR $R0
  StrCpy $R0 "$R0\${PRODUCTSAFE}.exe Main Window Class"

  FindWindow $R1 "$R0"
  IntCmp $R1 0 lbl_install_skip_closerunning
  Sleep 4000
  ExecWait "$\"$INSTDIR\${PRODUCTSAFE}.exe$\" /close"
  Sleep 6000

  lbl_install_start_closeall:
    FindWindow $R1 "$R0"
    IntCmp $R1 0 lbl_install_skip_closerunning
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION $(TEXT_AlreadyRunning) /SD IDCANCEL IDRETRY lbl_install_start_closeall
    Abort

  lbl_install_skip_closerunning:

  ;--------------------------------
  ;Don't install on Windows NT 3.x
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $R0 "" lbl_notwinnt
  StrCpy $R0 $R0 1
  StrCmp $R0 '3' lbl_oldwinnt
  Goto lbl_notwinnt
  lbl_oldwinnt:
    MessageBox MB_OK|MB_ICONEXCLAMATION $(TEXT_WindowsOutdated) /SD IDOK
    Abort
  lbl_notwinnt:

  ;--------------------------------
  ;MSIE 5.5 or higher required
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Internet Explorer" "Version"
  IfErrors 0 lbl_456
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Internet Explorer" "W2kVersion"
  IfErrors 0 lbl_456
  ClearErrors
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Internet Explorer" "Build"
  IfErrors lbl_update lbl_456
  lbl_456:
    StrCpy $R0 $R0 3
    StrCmp $R0 '5.0' lbl_update
    StrCpy $R0 $R0 1
    StrCmp $R0 '4' lbl_update
    Goto lbl_done
  lbl_update:
    MessageBox MB_YESNO|MB_ICONQUESTION $(TEXT_InternetExplorer) /SD IDNO IDNO NoDownload
      ExecShell "open" "http://www.microsoft.com/windows/ie/downloads/default.mspx"
    NoDownload:
    Abort
  lbl_done:

FunctionEnd

;--------------------------------
;Post-install checks

Function .onInstSuccess

  Call GetParameters
  Pop $R0

  StrCmp $R0 "/S autoupdate" 0 lbl_silent_autorun_skip
    ExecWait "$\"$INSTDIR\${PRODUCTSAFE}.exe$\""
  lbl_silent_autorun_skip:

  StrCpy $R1 $R0 5
  StrCmp $R1 "/sspi" 0 lbl_sspi_skip
    StrCpy $R1 $R0 256 6
    WriteRegStr HKCU "Software\${PRODUCTSAFE}\Settings" "sspiserver" "$R1"
    WriteRegStr HKLM "Software\${PRODUCTSAFE}\Settings" "sspiserver" "$R1"
  lbl_sspi_skip:

  StrCpy $R1 $R0 8
  StrCmp $R1 "/S /sspi" 0 lbl_silent_sspi_skip
    StrCpy $R1 $R0 256 9
    WriteRegStr HKCU "Software\${PRODUCTSAFE}\Settings" "sspiserver" "$R1"
    WriteRegStr HKLM "Software\${PRODUCTSAFE}\Settings" "sspiserver" "$R1"
  lbl_silent_sspi_skip:

FunctionEnd

;--------------------------------
;Descriptions

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCopyRequired}     $(DESC_SecCopyRequired)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCreateStartMenu}  $(DESC_SecCreateStartMenu)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCreateQuickstart} $(DESC_SecCreateQuickstart)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCreateDesktop}    $(DESC_SecCreateDesktop)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCreateAutoStart}  $(DESC_SecCreateAutoStart)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
