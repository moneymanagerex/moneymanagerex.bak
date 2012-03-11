﻿; Initial Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; Money Manager Ex InnoSetup script
;
; This file encoded as UTF-8.
; UTF-8 enables custom messages in other languages.
; Initial script modified using various strings and ideas by previous developers

; Copyright (C) 2006 Madhan Kanagavel
; Copyright (C) 2009 VaDiM
; Copyright (C) 2011-2012 Stefano Giorgio

#define MyAppName "MoneyManagerEX"
#define MyAppVersion "0.9.9.0"
#define MyAppPublisher "CodeLathe, LLC"
#define MyAppURL "http://www.codelathe.com/mmex"
#define MyAppExeName "mmex.exe"

;===============================================================================
; Local definitions specifically designed for my setup 
#define mmex_svn_path "C:\Users\Stef\Documents\SVN\MoneyManagerEX\trunk\mmex"
#define mmex_local_output_filename "mmex_0.9.9.0_win32_setup"
#define mmex_local_output_path "C:\Users\Stef\Documents\MMEX-RELEASE" 
#define mmex_install_root "\mmex_0.9.9.0_win32_portable\MoneyManagerEx"

;===============================================================================

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
; Development ID
AppId={{37153D93-6D91-4763-82BB-0DF646211ED0}
; Release ID
;AppId={{2C48DC11-E113-4912-8AFC-366D1918101E}

AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}

LicenseFile={#mmex_svn_path}\doc\license.txt
InfoBeforeFile={#mmex_svn_path}\README.TXT
InfoAfterFile={#mmex_svn_path}\doc\version.txt

OutputDir={#mmex_local_output_path}
OutputBaseFilename={#mmex_local_output_filename}
SetupIconFile={#mmex_svn_path}\resources\mmex.ico

Compression=lzma/Max
SolidCompression=true
ShowUndisplayableLanguages=true

[Languages]
Name: english; MessagesFile: compiler:Default.isl; 
Name: russian; MessagesFile: compiler:Languages\Russian.isl; InfoBeforeFile: {#mmex_svn_path}\README.RU; 
Name: basque; MessagesFile: compiler:Languages\Basque.isl; 
Name: brazilianportuguese; MessagesFile: compiler:Languages\BrazilianPortuguese.isl; 
Name: catalan; MessagesFile: compiler:Languages\Catalan.isl; 
Name: czech; MessagesFile: compiler:Languages\Czech.isl; 
Name: danish; MessagesFile: compiler:Languages\Danish.isl; 
Name: dutch; MessagesFile: compiler:Languages\Dutch.isl; 
Name: finnish; MessagesFile: compiler:Languages\Finnish.isl; 
Name: french; MessagesFile: compiler:Languages\French.isl; 
Name: german; MessagesFile: compiler:Languages\German.isl; 
Name: hebrew; MessagesFile: compiler:Languages\Hebrew.isl; 
Name: hungarian; MessagesFile: compiler:Languages\Hungarian.isl; 
Name: italian; MessagesFile: compiler:Languages\Italian.isl; 
Name: japanese; MessagesFile: compiler:Languages\Japanese.isl; 
Name: norwegian; MessagesFile: compiler:Languages\Norwegian.isl; 
Name: polish; MessagesFile: compiler:Languages\Polish.isl; 
Name: portuguese; MessagesFile: compiler:Languages\Portuguese.isl; 
Name: slovak; MessagesFile: compiler:Languages\Slovak.isl; 
Name: slovenian; MessagesFile: compiler:Languages\Slovenian.isl; 
Name: spanish; MessagesFile: compiler:Languages\Spanish.isl; 


[Types]
Name: full;    Description: "Full Installation"; 
Name: minimal; Description: "Minimal Installation"; 
Name: custom;  Description: "Custom Installation"; Flags: IsCustom; 

[Components]
Name: program; Description: "Program Files"; Types: full minimal custom; Flags: fixed; 
Name: help; Description: "Help files"; Types: full minimal; 

; Add language component here then add language file in files section
Name: lang; Description: Languages; Types: full; 
Name: lang\english; Description: English; Types: full; 
Name: lang\arabic; Description: Arabic; Types: full; 
Name: lang\bulgarian; Description: Bulgarian; Types: full; 
Name: lang\chinese_chs; Description: "Chinese Chs"; Types: full; 
Name: lang\chinese_zh; Description: "Chinese Zh"; Types: full; 
Name: lang\croatian; Description: Croatian; Types: full; 
Name: lang\czech; Description: Czech; Types: full; 
Name: lang\ducth; Description: Dutch; Types: full; 
Name: lang\dutch_be; Description: "Dutch Be"; Types: full; 
Name: lang\english_uk; Description: "English UK"; Types: full; 
Name: lang\french; Description: French; Types: full; 
Name: lang\german; Description: German; Types: full; 
Name: lang\greek; Description: Greek; Types: full; 
Name: lang\hebrew; Description: Hebrew; Types: full; 
Name: lang\hungarian; Description: Hungarian; Types: full; 
Name: lang\indonesian; Description: Indonesian; Types: full; 
Name: lang\italian; Description: Italian; Types: full; 
Name: lang\latvian; Description: Latvian; Types: full; 
Name: lang\norwegian; Description: Norwegian; Types: full; 
Name: lang\polish; Description: Polish; Types: full; 
Name: lang\portuguese; Description: Portuguese; Types: full; 
Name: lang\portuguese_portugal; Description: "Portuguese Portugal"; Types: full; 
Name: lang\romanian; Description: Romanian; Types: full; 
Name: lang\russian; Description: Russian; Types: full; 
Name: lang\serbo_croatian; Description: "Serbo Croatian"; Types: full; 
Name: lang\serbian; Description: Serbian; Types: full; 
Name: lang\slovak; Description: Slovak; Types: full; 
Name: lang\slovenian; Description: Slovenian; Types: full; 
Name: lang\spanish; Description: Spanish; Types: full; 
Name: lang\swedish; Description: Swedish; Types: full; 
Name: lang\tamil; Description: Tamil; Types: full; 
Name: lang\turkish; Description: Turkish; Types: full; 
Name: lang\ukrainian; Description: Ukrainian; Types: full; 
Name: lang\vietnamese; Description: Vietnamese; Types: full; 


[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; 
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked; 


[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

; MMEX Executable files 
Source: {#mmex_local_output_path}{#mmex_install_root}\bin\mmex.exe; DestDir: {app}\bin; Flags: ignoreversion; Components: program; 
Source: {#mmex_local_output_path}{#mmex_install_root}\bin\msvcp100.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: program; 
Source: {#mmex_local_output_path}{#mmex_install_root}\bin\msvcr100.dll; DestDir: {app}\bin; Flags: ignoreversion; Components: program; 

; MMEX Root files
Source: {#mmex_svn_path}\doc\contrib.txt; DestDir: {app}; Flags: ignoreversion; Components: program; 
Source: {#mmex_svn_path}\doc\license.txt; DestDir: {app}; Flags: ignoreversion; Components: program; 
Source: {#mmex_svn_path}\doc\version.txt; DestDir: {app}; Flags: ignoreversion; Components: program; 

; MMEX Resource files
Source: {#mmex_svn_path}\resources\currency_seed.db3; DestDir: {app}\res; Flags: ignoreversion; 
Source: {#mmex_svn_path}\resources\kaching.wav; DestDir: {app}\res; Flags: ignoreversion; 
Source: {#mmex_svn_path}\resources\mmex.ico; DestDir: {app}\res; Flags: ignoreversion; 
Source: {#mmex_svn_path}\resources\splash.png; DestDir: {app}\res; Flags: ignoreversion; 

; MMEX Root files - language dependant
Source: {#mmex_svn_path}\README.TXT; DestDir: {app}; Flags: ignoreversion; Components: program; Languages: english; 
Source: {#mmex_svn_path}\README.RU; DestDir: {app}; Flags: ignoreversion; Languages: russian; DestName: README.TXT; 

; MMEX Language Files - Determined by Component
Source: {#mmex_svn_path}\po\arabic.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\arabic; 
Source: {#mmex_svn_path}\po\bulgarian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\bulgarian; 
Source: {#mmex_svn_path}\po\chinese_chs.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\chinese_chs; 
Source: {#mmex_svn_path}\po\chinese_zh.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\chinese_zh; 
Source: {#mmex_svn_path}\po\croatian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\croatian; 
Source: {#mmex_svn_path}\po\czech.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\czech; 
Source: {#mmex_svn_path}\po\dutch.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\ducth; 
Source: {#mmex_svn_path}\po\dutch_be.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\dutch_be; 
Source: {#mmex_svn_path}\po\english.*; DestDir: {app}\po; Flags: ignoreversion; Components: help; Languages: english; 
Source: {#mmex_svn_path}\po\english-uk.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\english_uk; 
Source: {#mmex_svn_path}\po\french.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\french; 
Source: {#mmex_svn_path}\po\german.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\german; 
Source: {#mmex_svn_path}\po\greek.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\greek; 
Source: {#mmex_svn_path}\po\hebrew.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\hebrew; 
Source: {#mmex_svn_path}\po\hungarian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\hungarian; 
Source: {#mmex_svn_path}\po\indonesian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\indonesian; 
Source: {#mmex_svn_path}\po\italian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\italian; 
Source: {#mmex_svn_path}\po\latvian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\latvian; 
Source: {#mmex_svn_path}\po\norwegian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\norwegian; 
Source: {#mmex_svn_path}\po\polish.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\polish; 
Source: {#mmex_svn_path}\po\portuguese.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\portuguese; 
Source: {#mmex_svn_path}\po\portuguese_portugal.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\portuguese_portugal; 
Source: {#mmex_svn_path}\po\romanian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\romanian; 
Source: {#mmex_svn_path}\po\russian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\russian; 
Source: {#mmex_svn_path}\po\serbian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\serbian; 
Source: {#mmex_svn_path}\po\serbo-croatian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\serbo_croatian; 
Source: {#mmex_svn_path}\po\slovak.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\slovak; 
Source: {#mmex_svn_path}\po\slovenian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\slovenian; 
Source: {#mmex_svn_path}\po\spanish.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\spanish; 
Source: {#mmex_svn_path}\po\swedish.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\swedish; 
Source: {#mmex_svn_path}\po\tamil.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\tamil; 
Source: {#mmex_svn_path}\po\turkish.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\turkish; 
Source: {#mmex_svn_path}\po\ukrainian.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\ukrainian; 
Source: {#mmex_svn_path}\po\vietnamese.*; DestDir: {app}\po; Flags: ignoreversion; Components: lang\vietnamese; 

; MMEX Help - Root Help files 
Source: {#mmex_svn_path}\doc\help\*.html; DestDir: {app}\help; Flags: ignoreversion; Components: help;
Source: {#mmex_svn_path}\doc\help\*.png; DestDir: {app}\help; Flags: ignoreversion; Components: help; 
 
; MMEX Help - Help Directories - Language dependant
Source: {#mmex_local_output_path}{#mmex_install_root}\help\french\*; DestDir: {app}\help\french; Flags: ignoreversion recursesubdirs createallsubdirs; Components: lang\french; 
Source: {#mmex_local_output_path}{#mmex_install_root}\help\italian\*; DestDir: {app}\help\italian; Flags: ignoreversion recursesubdirs createallsubdirs; Components: lang\italian; 
Source: {#mmex_local_output_path}{#mmex_install_root}\help\polish\*; DestDir: {app}\help\polish; Flags: ignoreversion recursesubdirs createallsubdirs; Components: lang\polish; 
Source: {#mmex_local_output_path}{#mmex_install_root}\help\russian\*; DestDir: {app}\help\russian; Flags: ignoreversion recursesubdirs createallsubdirs; Components: lang\russian; 
Source: {#mmex_local_output_path}{#mmex_install_root}\help\spanish\*; DestDir: {app}\help\spanish; Flags: ignoreversion recursesubdirs createallsubdirs; Components: lang\spanish; 


[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\bin\{#MyAppExeName}; WorkingDir: {app}; 
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}; 
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: {app}\bin\{#MyAppExeName}; 

[Run]
Filename: {app}\bin\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent; WorkingDir: {app}; 

[UninstallDelete]
Type: files; Name: "{userappdata}\{#MyAppName}\mmexini.db3"
Type: dirifempty; Name: "{userappdata}\{#MyAppName}"

[Messages]
english.WelcomeLabel1=Welcome to [name] Setup
russian.WelcomeLabel1=Вас приветствует инсталлятор [name]
