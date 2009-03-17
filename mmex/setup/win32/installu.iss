; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define Version " 0.9.4.2"
#define FileVersion "_0_9_4_2u"

#define MyAppName "Money Manager Ex"
#define MyAppVerName "Money Manager Ex"
#define MyAppPublisher "CodeLathe LLC"
#define MyAppURL "http://www.codelathe.com/mmex"
#define MyAppExeName "mmex.exe"
#define MyAppUrlName "mmex.url"

[Setup]
AppName={#MyAppName}
AppVerName={#MyAppVerName}{#Version}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={userappdata}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=..\..\runtime\license.txt
OutputBaseFilename=mmgrex_setup{#FileVersion}
Compression=lzma
SolidCompression=yes
SetupIconFile=..\..\runtime\mmex.ico

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: ..\..\runtime\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
Source: ..\..\mmex_unicode_release\mmex.exe; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs
; Source: ..\..\mmex_unicode_release\mmex.pdb; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs

[INI]

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; IconFilename: {app}\mmex.exe; WorkingDir: {app}; IconIndex: 0
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}
Name: {userdesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; IconFilename: {app}\mmex.exe; WorkingDir: {app}; IconIndex: 0
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon; WorkingDir: {app}; IconFilename: {app}\mmex.exe; IconIndex: 0

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; Tasks: ; Languages: 

[UninstallDelete]
Type: files; Name: {app}\mmexini.db3; Tasks: ; Languages: 
Type: files; Name: {app}\*.png; Tasks: ; Languages: 
Name: {app}\*.txt; Type: files

[Messages]
WelcomeLabel1=Welcome to [name] Setup for Windows
