; This file is part of Pandion instant messenger
; Copyright (c) 2001-2009 Sebastiaan Deckers
; License: GNU General Public License version 3 or later

;--------------------------------
;Pre-Uninstall Checks

Function un.onInit

  ;--------------------------------
  ;Check the parameters
  Call un.GetParameters
  Pop $R0

  StrCmp $R0 "/hide" 0 lbl_uninstall_skip_hide
    Delete "$SMPROGRAMS\${PRODUCTSAFE}.lnk"
    Delete "$QUICKLAUNCH\${PRODUCTSAFE}.lnk"
    Delete "$DESKTOP\${PRODUCTSAFE}.lnk"
    Delete "$SMSTARTUP\${PRODUCTSAFE}.lnk"
    WriteRegDWORD HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" IconsVisible 0
    Abort
  lbl_uninstall_skip_hide:

  StrCmp $R0 "/show" 0 lbl_uninstall_skip_show
    SetOverwrite try
    CreateShortCut "$SMPROGRAMS\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
    CreateShortCut "$QUICKLAUNCH\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
    CreateShortCut "$DESKTOP\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "" "$INSTDIR\images\brand\default.ico" 0
    CreateShortCut "$SMSTARTUP\${PRODUCTSAFE}.lnk" "$INSTDIR\${PRODUCTSAFE}.exe" "/minimized" "$INSTDIR\images\brand\default.ico" 0
    WriteRegDWORD HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}\InstallInfo" IconsVisible 1
    Abort
  lbl_uninstall_skip_show:

  StrCmp $R0 "/reinstall" 0 lbl_uninstall_skip_reinstall

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

    Abort
  lbl_uninstall_skip_reinstall:

FunctionEnd

;--------------------------------
;All users or current user

Function un.SetAllUsers

  ClearErrors
  UserInfo::GetName
  IfErrors lbl_allusers_done
  Pop $0
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" 0 +2
    SetShellVarContext all
  lbl_allusers_done:

FunctionEnd

;--------------------------------
; un.GetParameters
; input, none
; output, top of stack (replaces, with e.g. whatever)
; modifies no other variables.

Function un.GetParameters
  Push $R0
  Push $R1
  Push $R2
  StrCpy $R0 $CMDLINE 1
  StrCpy $R1 '"'
  StrCpy $R2 1
  StrCmp $R0 '"' loop
    StrCpy $R1 ' ' ; we're scanning for a space instead of a quote
  loop:
    StrCpy $R0 $CMDLINE 1 $R2
    StrCmp $R0 $R1 loop2
    StrCmp $R0 "" loop2
    IntOp $R2 $R2 + 1
    Goto loop
  loop2:
    IntOp $R2 $R2 + 1
    StrCpy $R0 $CMDLINE 1 $R2
    StrCmp $R0 " " loop2
  StrCpy $R0 $CMDLINE "" $R2
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCTSAFE}"
  DeleteRegKey HKLM "SOFTWARE\Clients\IM\${PRODUCTSAFE}"
  DeleteRegKey HKCU "Software\${PRODUCTSAFE}"
  DeleteRegKey HKLM "Software\${PRODUCTSAFE}"

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

  SetShellVarContext current
  Delete "$SMPROGRAMS\${PRODUCTSAFE}.lnk"
  Delete "$QUICKLAUNCH\${PRODUCTSAFE}.lnk"
  Delete "$DESKTOP\${PRODUCTSAFE}.lnk"
  Delete "$SMSTARTUP\${PRODUCTSAFE}.lnk"
  Call un.SetAllUsers
  Delete "$SMPROGRAMS\${PRODUCTSAFE}.lnk"
  Delete "$QUICKLAUNCH\${PRODUCTSAFE}.lnk"
  Delete "$DESKTOP\${PRODUCTSAFE}.lnk"
  Delete "$SMSTARTUP\${PRODUCTSAFE}.lnk"

  RMDir /r "$INSTDIR"

  ; ; ;  Try to find and remove the profiles  ; ; ;
  ClearErrors
  ReadRegStr $R0 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "AppData"
  IfErrors      lbl_profiles_second lbl_profiles_dircheck
  StrCmp $R0 "" lbl_profiles_second lbl_profiles_dircheck

  lbl_profiles_second:
  ClearErrors
  ReadRegStr $R0 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Personal"
  IfErrors      lbl_profiles_skip lbl_profiles_dircheck
  StrCmp $R0 "" lbl_profiles_skip lbl_profiles_dircheck

  lbl_profiles_dircheck:
  IfFileExists "$R0\${PRODUCTSAFE}" 0 lbl_profiles_skip

  MessageBox MB_YESNO|MB_ICONQUESTION $(un.TEXT_RemoveProfiles) /SD IDNO IDNO lbl_profiles_skip
    RMDir /r "$R0\${PRODUCTSAFE}"
  lbl_profiles_skip:

  ; ; ;  Check if the main stuff is gone  ; ; ;
  Sleep 666 ;-)
  IfFileExists "$INSTDIR" 0 lbl_all_removed
    MessageBox MB_OK|MB_ICONEXCLAMATION $(un.TEXT_RemoveDirectory) /SD IDOK
  lbl_all_removed:

SectionEnd
