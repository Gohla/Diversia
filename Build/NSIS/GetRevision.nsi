!include "TextFunc.nsh"

OutFile "GetRevision.exe"
SilentInstall silent
 
Section
 
 ## Get client revision
 ${LineRead} "..\..\.svn\entries" "11" $R1
 ${TrimNewLines} "$R1" $R1

 ## Write it to a !define for use in main script
 FileOpen $R0 "$EXEDIR\Revision.txt" w
  FileWrite $R0 '!define Revision "$R1"'
 FileClose $R0
 
SectionEnd