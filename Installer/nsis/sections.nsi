; This file is part of Pandion instant messenger
; Copyright (c) 2001-2009 Sebastiaan Deckers
; License: GNU General Public License version 3 or later

;--------------------------------
;Installer Sections

Section "${PRODUCT} ${VERSION}" SecCopyRequired
  SectionIn RO

  SetOverwrite on
  SetOutPath "$INSTDIR"

  ;-----------------------
  ;MSXML3 SP2

  IfFileExists "$SYSDIR\msxml3.dll" lbl_msxml3
    !ifdef ONLINE_INSTALL
      NSISdl::download "${ONLINE_INSTALL}/msxml3-sp2.zip" "$INSTDIR\msxml3-sp2.zip"
      Pop $R0
      StrCmp $R0 "success" lbl_msxml3_continue
        Abort $(TEXT_DownloadFailed)
      lbl_msxml3_continue:
    !else
      SetOutPath "$INSTDIR"
      File "download\msxml3-sp2.zip"
    !endif
    !insertmacro ZIPDLL_EXTRACT "$INSTDIR\msxml3-sp2.zip" "$SYSDIR" "<ALL>"
    Delete "$INSTDIR\msxml3-sp2.zip"
    RegDLL "$SYSDIR\msxml3.dll"
    RegDLL "$SYSDIR\msxml3a.dll"
    RegDLL "$SYSDIR\msxml3r.dll"
  lbl_msxml3:

  ;-----------------------
  ;Source files

  SetOutPath "$INSTDIR"
  File "${SOURCEDIR}\License.rtf"
  File "${SOURCEDIR}\${PRODUCTSAFE}.exe"
  ExecWait "$\"$INSTDIR\${PRODUCTSAFE}.exe$\" /RegServer"

  RMDir /r "$INSTDIR\backgrounds"
  RMDir /r "$INSTDIR\emoticons"
  RMDir /r "$INSTDIR\images"
  RMDir /r "$INSTDIR\plugins"

  File /r "${SOURCEDIR}\avatars"
  File /r "${SOURCEDIR}\css"
  File /r "${SOURCEDIR}\emoticons"
  File /r "${SOURCEDIR}\images"
  File /r "${SOURCEDIR}\js"
  File /r "${SOURCEDIR}\languages"
  File /r "${SOURCEDIR}\settings"
  File /r "${SOURCEDIR}\src"
  File /nonfatal /r "${SOURCEDIR}\backgrounds"
  File /nonfatal /r "${SOURCEDIR}\plugins"
  File /nonfatal /r "${SOURCEDIR}\sounds"

  IfFileExists "$EXEDIR\bookmarks.xml" 0 +2
    CopyFiles /SILENT /FILESONLY "$EXEDIR\bookmarks.xml" "$INSTDIR\settings\bookmarks.xml"
  IfFileExists "$EXEDIR\override.xml" 0 +2
    CopyFiles /SILENT /FILESONLY "$EXEDIR\override.xml" "$INSTDIR\settings\override.xml"
  IfFileExists "$EXEDIR\servers.xml" 0 +2
    CopyFiles /SILENT /FILESONLY "$EXEDIR\servers.xml" "$INSTDIR\settings\servers.xml"
  IfFileExists "$EXEDIR\default.xml" 0 +2
    CopyFiles /SILENT /FILESONLY "$EXEDIR\default.xml" "$INSTDIR\settings\default.xml"
  DetailPrint " "

  ;------------------------------
  ;Program Access and Defaults

  ; ; ;  Only install this on Windows 2000 or later  ; ; ;
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  StrCmp $R0 "" lbl_not_win2k_or_higher

  StrCpy $R0 $R0 1
  StrCmp $R0 '5' lbl_win2k_or_higher
  Goto lbl_not_win2k_or_higher

  lbl_win2k_or_higher:
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}" "" "${PRODUCT}"
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}" "LocalizedString" "${PRODUCT}"
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\DefaultIcon" "" "$INSTDIR\images\brand\default.ico"
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" ReinstallCommand '"$INSTDIR\Uninstall.exe" /reinstall'
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" HideIconsCommand '"$INSTDIR\Uninstall.exe" /hide'
    WriteRegStr HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" ShowIconsCommand '"$INSTDIR\Uninstall.exe" /show'
    WriteRegDWORD HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" IconsVisible 1
  lbl_not_win2k_or_higher:

  ;------------------------------
  ;Protocol and file extension handlers

  DeleteRegKey HKCR "xmpp"
  DeleteRegKey HKCR ".jisp"
  DeleteRegKey HKCR "JISPfile"
  DeleteRegKey HKCR ".pdn"
  DeleteRegKey HKCR "PDNfile"
  DeleteRegKey HKCR ".xmpp"
  DeleteRegKey HKCR "XMPPfile"
  DeleteRegKey HKCR "MIME\Database\Content Type\application/jabber+xml"
  DeleteRegKey HKCR "MIME\Database\Content Type\application/vnd.jisp"
  DeleteRegKey HKCR "MIME\Database\Content Type\application/x-pdn+xml"

  ; "xmpp:" URI scheme
  WriteRegStr HKCR "xmpp" "" "URL:XMPP Address"
  WriteRegStr HKCR "xmpp" "URL Protocol" ""
  WriteRegStr HKCR "xmpp\DefaultIcon" "" '"$INSTDIR\images\brand\default.ico"'
  WriteRegStr HKCR "xmpp\shell\open\command" "" '"$INSTDIR\${PRODUCTSAFE}.exe" %1'

  ; File extension and MIME type
  WriteRegStr HKCR ".jisp" "" "JISPfile"
  WriteRegStr HKCR ".jisp" "Content Type" "application/vnd.jisp"
  WriteRegStr HKCR "MIME\Database\Content Type\application/vnd.jisp" "Extension" ".jisp"

  ; File class information
  WriteRegStr HKCR "JISPfile" "" "Jabber Icon Style Package"
  WriteRegDword HKCR "JISPfile" "EditFlags"    0x00010000
  WriteRegDword HKCR "JISPfile" "BrowserFlags" 0x00000008
  WriteRegStr HKCR "JISPfile\DefaultIcon" "" '"$INSTDIR\images\brand\default.ico"'
  WriteRegStr HKCR "JISPfile\shell" "" "Open"
  WriteRegStr HKCR "JISPfile\shell\Open\command" "" '"$INSTDIR\${PRODUCTSAFE}.exe" %1'

  ; File extension and MIME type
  WriteRegStr HKCR ".pdn" "" "PDNfile"
  WriteRegStr HKCR ".pdn" "Content Type" "application/x-pdn+xml"
  WriteRegStr HKCR "MIME\Database\Content Type\application/x-pdn+xml" "Extension" ".pdn"

  ; File class information
  WriteRegStr HKCR "PDNfile" "" "PDN"
  WriteRegDword HKCR "PDNfile" "EditFlags"    0x00010000
  WriteRegDword HKCR "PDNfile" "BrowserFlags" 0x00000008
  WriteRegStr HKCR "PDNfile\DefaultIcon" "" '"$INSTDIR\images\brand\default.ico"'
  WriteRegStr HKCR "PDNfile\shell" "" "Open"
  WriteRegStr HKCR "PDNfile\shell\Open\command" "" '"$INSTDIR\${PRODUCTSAFE}.exe" %1'

  ; File extension and MIME type
  WriteRegStr HKCR ".xmpp" "" "XMPPfile"
  WriteRegStr HKCR ".xmpp" "Content Type" "application/jabber+xml"
  WriteRegStr HKCR "MIME\Database\Content Type\application/jabber+xml" "Extension" ".xmpp"

  ; File class information
  WriteRegStr HKCR "XMPPfile" "" "eXtensible Messaging and Presence Protocol"
  WriteRegDword HKCR "XMPPfile" "EditFlags"    0x00010000
  WriteRegDword HKCR "XMPPfile" "BrowserFlags" 0x00000008
  WriteRegStr HKCR "XMPPFile\DefaultIcon" "" '"$INSTDIR\images\brand\default.ico"'
  WriteRegStr HKCR "XMPPfile\shell" "" "Open"
  WriteRegStr HKCR "XMPPfile\shell\Open\command" "" '"$INSTDIR\${PRODUCTSAFE}.exe" %1'

  ;-----------------------
  ;Create Uninstaller

  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCTSAFE}" "DisplayName" "${PRODUCT}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCTSAFE}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section $(ITEM_SecCreateStartMenu) SecCreateStartMenu
  SetShellVarContext current
  Delete "$SMPROGRAMS\${PRODUCTSAFE}.lnk"
  Call SetAllUsers
  SetOverwrite try
  CreateShortCut "$SMPROGRAMS\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
SectionEnd

Section $(ITEM_SecCreateQuickstart) SecCreateQuickstart
  SetShellVarContext current
  Delete "$QUICKLAUNCH\${PRODUCTSAFE}.lnk"
  Call SetAllUsers
  SetOverwrite try
  CreateShortCut "$QUICKLAUNCH\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
SectionEnd

Section $(ITEM_SecCreateDesktop) SecCreateDesktop
  SetShellVarContext current
  Delete "$DESKTOP\${PRODUCTSAFE}.lnk"
  Call SetAllUsers
  SetOverwrite try
  CreateShortCut "$DESKTOP\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
SectionEnd

Section $(ITEM_SecCreateAutoStart) SecCreateAutoStart
  SetShellVarContext current
  Delete "$SMSTARTUP\${PRODUCTSAFE}.lnk"
  Call SetAllUsers
  SetOverwrite try
  CreateShortCut "$SMSTARTUP\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "/minimized" "$INSTDIR\images\brand\default.ico" 0
SectionEnd
