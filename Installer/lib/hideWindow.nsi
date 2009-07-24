; This file is part of Pandion instant messenger
; Copyright (c) 2001-2009 Sebastiaan Deckers
; License: GNU General Public License version 3 or later

;Workaround for bug in NSIS MUI2
;Hide dialog while NSIS freezes after processing Sections

Function hideWindow
	HideWindow
FunctionEnd
