;--------------------------------
;All users or current user

Function SetAllUsers

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
