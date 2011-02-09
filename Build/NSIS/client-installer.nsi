;Diversia client installer

;--------------------------------
;Get revision number

  !system "GetRevision.exe"
  !include "Revision.txt"

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Diversia client"
  OutFile "Diversia_client_r${Revision}.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES32\Diversia\"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Diversia\Client" "Install_Dir"

  ;Request admin application privileges for Windows Vista/7
  RequestExecutionLevel admin

;--------------------------------
; Detect previous installations

Function .onInit

  ReadRegStr $0 HKCU "Software\Diversia\Client" "Install_Dir"
  ReadRegStr $1 HKCU "Software\Diversia\Client" "Install_Rev"

  ${If} $1 == ""
    Goto done
  ${ElseIf} $1 == ${Revision}
    MessageBox MB_YESNO "You have already installed this version of the Diversia client, would you like to uninstall first? (No will quit the installer)" IDYES true1 IDNO false1
    true1:
      ExecWait '"$0\Uninstall.exe" _?=$0' $2
      Goto checkUninstall
    false1:
      Quit
  ${ElseIf} $1 < ${Revision}
    MessageBox MB_YESNO "You have an old version (rev$1) of the Diversia client already installed, the old version must be uninstalled first. Would you like to do this? (No will quit the installer)" IDYES true2 IDNO false2
    true2:
      ExecWait '"$0\Uninstall.exe" _?=$0' $2
      Goto checkUninstall
    false2:
      Quit
  ${ElseIf} $1 > ${Revision}
    MessageBox MB_YESNO "You have a newer version (rev$1) of the Diversia client already installed, the newer version must be uninstalled first. Would you like to do this? (No will quit the installer)" IDYES true3 IDNO false3
    true3:
      ExecWait '"$0\Uninstall.exe" _?=$0' $2
      Goto checkUninstall
    false3:
      Quit
  ${Else}
    Goto done
  ${EndIf}

  checkUninstall:
    ; Check if user actually completed the uninstall (return code should be 0)
    ${If} $2 != 0
      Quit
    ${EndIf}

  done:

FunctionEnd

;--------------------------------
;Variables

  Var StartMenuFolder
  Var DirectXSetupError
  Var VS2010RedistSetupError

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Diversia\Client" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
; DefaultClient section
Section "DefaultClient" SecDefaultClient

  ; Client files
  SetOutPath "$INSTDIR\DefaultClient"
  File /r ..\..\DefaultClient\bin\ReleaseWin32\Diversia-DefaultClient-DbgInfo.exe
  File /r ..\..\DefaultClient\bin\ReleaseWin32\*.dll
  File /r /x cache /x .svn ..\..\DefaultClient\media

  ; Start menu shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Diversia DefaultClient.lnk" "$INSTDIR\DefaultClient\Diversia-DefaultClient-DbgInfo.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
; QtOgreEditor section
Section "QtOgreEditor" SecQtOgreEditor

  ; Client files
  SetOutPath "$INSTDIR\QtOgreEditor"
  File /r ..\..\QtOgreEditor\bin\ReleaseWin32\Diversia-QtOgreEditor-DbgInfo.exe
  File /r ..\..\QtOgreEditor\bin\ReleaseWin32\*.dll
  File /r ..\..\QtOgreEditor\qt\MainWindow.rcc
  File /r /x cache /x .svn ..\..\QtOgreEditor\media

  ; Start menu shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Diversia QtOgreEditor.lnk" "$INSTDIR\QtOgreEditor\Diversia-QtOgreEditor-DbgInfo.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
; Core section
Section "Core" SecCore

  ; Section is required
  SectionIn RO
  
  ; Store installation folder and revision number
  WriteRegStr HKCU "Software\Diversia\Client" "Install_Dir" $INSTDIR
  WriteRegStr HKCU "Software\Diversia\Client" "Install_Rev" ${Revision}
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Diversia\Client" "DisplayName" "Diversia client"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Diversia\Client" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Diversia\Client" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Diversia\Client" "NoRepair" 1

  ; Start menu shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
; DirectX section
Section "DirectX 11 redistributable" SecDirectX

  ; Copy to temp directory
  SetOutPath "$TEMP"
  File "dxwebsetup.exe"
  
  ; Execute setup
  DetailPrint "Running DirectX 11 redistributable setup..."
  ExecWait '"$TEMP\dxwebsetup.exe" /Q' $DirectXSetupError
  DetailPrint "Finished DirectX 11 redistributable setup."
  
  ; Destroy temp setup file
  Delete "$TEMP\dxwebsetup.exe"
  
  ; Reset output path
  SetOutPath "$INSTDIR"
  
SectionEnd

;--------------------------------
; DirectX section
Section "VS2010 redistributable" SecVS2010Redist

  ; Copy to temp directory
  SetOutPath "$TEMP"
  File "vcredist_x86.exe"
  
  ; Execute setup
  DetailPrint "Running VS2010 redistributable setup..."
  ExecWait '"$TEMP\vcredist_x86.exe" /Q' $VS2010RedistSetupError
  DetailPrint "Finished VS2010 redistributable setup."
  
  ; Destroy temp setup file
  Delete "$TEMP\vcredist_x86.exe"
  
  ; Reset output path
  SetOutPath "$INSTDIR"
  
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecCore ${LANG_ENGLISH} "The core files."
  LangString DESC_SecDefaultClient ${LANG_ENGLISH} "The Diversia client executables, required libraries and default media files."
  LangString DESC_SecQtOgreEditor ${LANG_ENGLISH} "The Diversia editor executables, required libraries and default media files."
  LangString DESC_SecDirectX ${LANG_ENGLISH} "DirectX 11 redistributable. Updates DirectX to the required version."
  LangString DESC_SecVS2010Redist ${LANG_ENGLISH} "Visual Studio 2010 redistributable. Required to run any Diversia application."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_SecCore)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDefaultClient} $(DESC_SecDefaultClient)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecQtOgreEditor} $(DESC_SecQtOgreEditor)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDirectX} $(DESC_SecDirectX)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecVS2010Redist} $(DESC_SecVS2010Redist)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ; Remove DefaultClient
  RMDir /r "$INSTDIR\DefaultClient\media"
  Delete "$INSTDIR\DefaultClient\*.dll"
  Delete "$INSTDIR\DefaultClient\Diversia-DefaultClient-DbgInfo.exe"
  Delete "$INSTDIR\DefaultClient\Diversia-DefaultClient-DbgInfo.exe.manifest"
  RMDir "$INSTDIR\DefaultClient"
  
  ; Remove QtOgreEditor
  RMDir /r "$INSTDIR\QtOgreEditor\media"
  Delete "$INSTDIR\QtOgreEditor\*.dll"
  Delete "$INSTDIR\QtOgreEditor\Diversia-QtOgreEditor.exe"
  DELETE "$INSTDIR\QtOgreEditor\qt\MainWindow.rcc"
  RMDir "$INSTDIR\QtOgreEditor"

  ; Delete start menu entries
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Diversia DefaultClient.lnk" 
  Delete "$SMPROGRAMS\$StartMenuFolder\Diversia QtOgreEditor.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  ; Delete registry entries
  DeleteRegKey HKCU "Software\Diversia\Client"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Diversia\Client"
  
  ; Remove core
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR\"

SectionEnd