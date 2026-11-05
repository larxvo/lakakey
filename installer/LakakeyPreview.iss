; Lakakey Windows Settings & 3D Mascot preview only.
; This installer is intentionally NOT named LakakeySetup.exe until IME works.
#define MyAppName "Lakakey Desktop Preview"
#define MyAppVersion "0.7.1-preview"
#define MyAppPublisher "Larx Vo"
#define MyAppURL "https://vovinhlac.com"
[Setup]
AppId={{F273568D-2595-42D4-BEC5-A6C62339D00A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppCopyright=Copyright (C) 2026 Larx Vo
DefaultDirName={localappdata}\Programs\LakakeyPreview
DefaultGroupName=Lakakey Preview
DisableProgramGroupPage=yes
OutputDir=output
OutputBaseFilename=LakakeyPreviewSetup-v0.7.1-win-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayIcon={app}\LakakeySettings.exe
SetupLogging=yes
[Files]
Source: "..\dist\LakakeySettings.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\dist\LakakeyMascot.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\dist\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\COPYRIGHT.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\PREVIEW_README_VI.txt"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\Lakakey Preview"; Filename: "{app}\LakakeySettings.exe"
Name: "{group}\Uninstall Lakakey Preview"; Filename: "{uninstallexe}"
[Run]
Filename: "{app}\LakakeySettings.exe"; Description: "Open Lakakey Desktop Preview"; Flags: nowait postinstall skipifsilent
[Code]
function InitializeSetup(): Boolean;
begin
  Result := MsgBox('Lakakey Desktop Preview is NOT yet a Vietnamese input method. The VN/EN toggle only changes the preview interface.' + #13#10 + #13#10 + 'Do you want to install this Settings and Mascot preview?', mbConfirmation, MB_YESNO) = IDYES;
end;
