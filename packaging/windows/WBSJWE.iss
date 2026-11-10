#define MyAppName "WBSJ WE"
#define MyAppVersion "0.2.0"
#define MyAppPublisher "WBSJ"
#define MyAppURL "https://wbsj.ir"
#define DesktopExe "WBSJ_WE.exe"
#define ServiceExe "WBSJ_WE_RecordingService.exe"

[Setup]
AppId={{2BCE81B7-2CF3-4DF0-9F7C-91D6B9E81F0A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\WBSJ WE
DefaultGroupName=WBSJ WE
OutputDir=..\..\dist
OutputBaseFilename=WBSJ_WE_Setup_{#MyAppVersion}_x64
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
PrivilegesRequired=admin
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\{#DesktopExe}
SetupIconFile=..\..\resources\WBSJ_WE.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Files]
Source: "..\..\build-windows\Release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\resources\WBSJ_WE.ico"; DestDir: "{app}\resources"; Flags: ignoreversion
Source: "..\..\config\service.example.json"; DestDir: "{app}\config"; Flags: onlyifdoesntexist
Source: "..\..\scripts\install-service.ps1"; DestDir: "{app}\scripts"; Flags: ignoreversion
Source: "..\..\scripts\uninstall-service.ps1"; DestDir: "{app}\scripts"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\WBSJ WE"; Filename: "{app}\{#DesktopExe}"
Name: "{autodesktop}\WBSJ WE"; Filename: "{app}\{#DesktopExe}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked

[Run]
Filename: "powershell.exe"; Parameters: "-NoProfile -ExecutionPolicy Bypass -File ""{app}\scripts\install-service.ps1"" -ServiceExe ""{app}\{#ServiceExe}"""; Flags: runhidden waituntilterminated; StatusMsg: "Installing WBSJ WE background service..."
Filename: "{app}\{#DesktopExe}"; Description: "Launch WBSJ WE"; Flags: postinstall nowait skipifsilent

[UninstallRun]
Filename: "powershell.exe"; Parameters: "-NoProfile -ExecutionPolicy Bypass -File ""{app}\scripts\uninstall-service.ps1"""; Flags: runhidden waituntilterminated; RunOnceId: "RemoveWBSJService"

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
end;
