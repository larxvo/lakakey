; Lakakey experimental native TSF installer. Copyright 2026 Larx Vo.
; CAUTION: internal alpha, Windows x64 only. Not production tested.
#define MyAppName "Lakakey Native TSF Alpha"
#define MyAppVersion "0.9.0-alpha"
#define MyPublisher "Larx Vo"
#define MyAppURL "https://vovinhlac.com"
#ifndef SourceDll
  #error SourceDll must be supplied from the Windows build workflow
#endif

[Setup]
AppId={{F1E9FD84-1D62-4793-9DD7-621B9A3223E4}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL=https://github.com/larxvo/lakakey/issues
AppUpdatesURL=https://github.com/larxvo/lakakey/releases
DefaultDirName={localappdata}\Programs\LakakeyNativeAlpha
DefaultGroupName=Lakakey Native Alpha
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=..\..\dist
OutputBaseFilename=LakakeyNativeAlphaSetup-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName=Lakakey Native TSF Alpha (Experimental)
CloseApplications=yes
RestartApplications=no
UsePreviousAppDir=no
SetupLogging=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#SourceDll}"; DestDir: "{app}"; DestName: "LakakeyTsfV2.dll"; Flags: ignoreversion
Source: "..\..\docs\TSF_V2_EXPERIMENT.md"; DestDir: "{app}"; DestName: "README_EXPERIMENTAL.txt"; Flags: ignoreversion

[Run]
; Per-user COM + TSF registration; do not silently activate/input-capture a keyboard.
Filename: "{sys}\regsvr32.exe"; Parameters: "/s ""{app}\LakakeyTsfV2.dll"""; StatusMsg: "Registering the experimental Windows Text Service..."; Flags: runhidden waituntilterminated

[UninstallRun]
; The file exists until UninstallRun has completed.
Filename: "{sys}\regsvr32.exe"; Parameters: "/s /u ""{app}\LakakeyTsfV2.dll"""; Flags: runhidden waituntilterminated

[Code]
function InitializeSetup(): Boolean;
var
  Response: Integer;
begin
  Response := MsgBox(
    'LAKAKEY EXPERIMENTAL INPUT METHOD' + #13#10 + #13#10 +
    'This installer registers an unverified Windows Text Services Framework keyboard. ' +
    'It may not work correctly in Word, Chrome, Notepad or VS Code.' + #13#10 +
    'It must NOT be used to type passwords or important documents. ' +
    'Keep another known working keyboard available.' + #13#10 + #13#10 +
    'Continue with the alpha installation?',
    mbConfirmation, MB_YESNO or MB_DEFBUTTON2);
  Result := Response = IDYES;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    MsgBox('Lakakey Native Alpha was installed. Restart the target application, ' +
      'then open Windows Settings > Time & language > Language & region ' +
      'to look for the experimental Vietnamese input method.' + #13#10 +
      'If it does not appear or typing is unreliable, uninstall Lakakey Native TSF Alpha immediately.' +
      #13#10 + #13#10 + 'Author: Larx Vo — vovinhlac.com',
      mbInformation, MB_OK);
end;
