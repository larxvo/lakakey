; Lakakey experimental native TSF installer. Copyright 2026 Larx Vo.
; CAUTION: internal alpha, Windows x64 only. Not production tested.
#define MyAppName "Lakakey Native TSF Alpha"
#define MyAppVersion "0.11.0-alpha"
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
InfoBeforeFile=..\..\docs\TSF_V2_EXPERIMENT.md

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
