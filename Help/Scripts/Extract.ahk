bTranslated := False
n := 0

; Read po
Loop, Read, C:\Graph\GraphHelp_Danish.po
  {
  If (A_LoopReadLine != "") {
    sBlock .= A_LoopReadLine "`n"
    If (SubStr(A_LoopReadLine, 1, 6) == "msgstr") 
      Area = msgstr
    If (Area == "msgstr") {
      search = "(.*)"
      RegExMatch(A_LoopReadLine, search, o)
      If (StrLen(o1) > 0) {
        bTranslated := True
        ;~MsgBox % A_LoopReadLine "`n" o1
        }
      }
    }
  Else {
    ;~MsgBox %sBlock%
    If (bTranslated == False) {
      sOut .= sBlock "`n"
      n++
      ;~MsgBox B
      }
    bTranslated := False
    sBlock =
    sMsgStr =
    Area =
    }
  }
If sMsgStr
  sOut .= sBlock "`n"

; Write po
FileAppend, %sOut%, C:\Graph\GraphHelp_Danish_%A_Now%.po
MsgBox %n% items to translate.
