;--------------------------------
; ChopAtLastIndexOf
;
; input:  top of stack   = string to search for
;         top of stack-1 = string to search in
; output: top of stack   = replaces with the portion of the string remaining
;
; Usage:
;   Push "this is a long ass string"
;   Push "ass"
;   Call ChopAtLastIndexOf
;   Pop $R0
;  ($R0 at this point is "this is a long ")

Function ChopAtLastIndexOf
Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrLen $R4 $R2
  IntOp $R4 $R4 - 1
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len (length of needle)
  ; $R4=cnt (begin at length of haystack - 1)
  ; $R5=tmp
  loop:
    StrCpy $R5 $R2 $R3 $R4
    StrCmp $R5 $R1 done
    StrCmp $R5 "" done
    IntOp $R4 $R4 - 1
    Goto loop
done:
  StrCpy $R1 $R2 $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
FunctionEnd
