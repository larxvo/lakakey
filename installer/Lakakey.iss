; Lakakey 0.14.1 — branded interactive Windows x64 installer
; Created by Larx Vo · https://vovinhlac.com
; Normal setup installs only Settings + tray. TSF research DLL is not bundled.

#define MyAppName "Lakakey"
#define MyAppVersion "0.14.1"
#define MyAppPublisher "Larx Vo"
#define MyAppURL "https://vovinhlac.com"
#define MyExeName "LakakeyDesktop.exe"

[Setup]
AppId={{72AFA3BA-E8A9-4BE0-8E0F-B8AF7132014D}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName=Lakakey {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppCopyright=Copyright (C) 2026 Larx Vo
DefaultDirName={localappdata}\Programs\Lakakey
DefaultGroupName=Lakakey
DisableProgramGroupPage=yes
DisableDirPage=yes
DisableWelcomePage=no
WizardStyle=modern
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=..\dist\windows
OutputBaseFilename=Lakakey-Setup-0.14.1-Windows-x64
SetupIconFile=..\assets\windows\lakakey.ico
UninstallDisplayIcon={app}\{#MyExeName}
Compression=lzma2
SolidCompression=yes
CloseApplications=yes
RestartApplications=no
SetupLogging=yes
MinVersion=10.0
VersionInfoVersion=0.14.1.0
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription=Lakakey Windows Installer
VersionInfoProductName=Lakakey
VersionInfoCopyright=Copyright (C) 2026 Larx Vo

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "vietnamese"; MessagesFile: "compiler:Languages\Vietnamese.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional tasks:"; Flags: unchecked

[Files]
Source: "..\desktop\installer\payload\LakakeyDesktop.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\desktop\installer\payload\README.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\Lakakey"; Filename: "{app}\{#MyExeName}"; IconFilename: "{app}\{#MyExeName}"
Name: "{autodesktop}\Lakakey"; Filename: "{app}\{#MyExeName}"; IconFilename: "{app}\{#MyExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyExeName}"; Description: "Launch Lakakey"; Flags: postinstall nowait skipifsilent

[UninstallRun]
Filename: "{app}\{#MyExeName}"; Parameters: "--quit"; Flags: runhidden; RunOnceId: "LakakeyStop"

[Code]
const
  LegacyUninstallKey = 'Software\Microsoft\Windows\CurrentVersion\Uninstall\LakakeyDesktopBeta';

procedure CurStepChanged(CurStep: TSetupStep);
var
  ExitCode: Integer;
  OldApp: String;
begin
  if CurStep = ssInstall then
  begin
    OldApp := ExpandConstant('{app}\LakakeyDesktop.exe');
    if FileExists(OldApp) then
      Exec(OldApp, '--quit', '', SW_HIDE, ewWaitUntilTerminated, ExitCode);
  end;
  if CurStep = ssPostInstall then
  begin
    { The older desktop-only installer used a separate uninstall entry.
      Clean it only after the new Inno install has completed. }
    if RegKeyExists(HKCU, LegacyUninstallKey) then
      RegDeleteKeyIncludingSubkeys(HKCU, LegacyUninstallKey);
    DeleteFile(ExpandConstant('{app}\UninstallLakakey.exe'));
  end;
end;
