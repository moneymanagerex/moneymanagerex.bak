; Money Manager Ex InnoSetup script

; This file encoded as UTF-8 with BOM.
; UTF-8 enables custom messages in such languages as Chinese, Japanese, etc.

; Copyright (C) 2006 Madhan Kanagavel
; Copyright (C) 2009 VaDiM

#ifndef AppExePath
        #error Use option -dAppExePath=path_to_your_exe
#endif

#ifndef BuildOpts
        #error Use option -dBuildOpts=value
#endif

#ifndef CpuArch
        #error Use option -dCpuArch=[x86|amd64|ia64]
#endif

#ifndef CrtDlls
        #error Use option -dCrtDlls=value
#endif

#ifndef Format
        #error Use option -dFormat=bakefile_format_var
#endif


#define AppURL "http://www.codelathe.com/mmex"
#define AppExeName ExtractFileName( AppExePath )
#define OutputDir  ExtractFilePath( AppExePath )


; information from .exe GetVersionInfo

#define AppName GetStringFileInfo( AppExePath, PRODUCT_NAME )
#define VersionInfo GetFileVersion( AppExePath )
#define Copyright GetFileCopyright( AppExePath )
#define Company GetFileCompany( AppExePath )

#define OutputBaseFilename GetStringFileInfo( AppExePath, INTERNAL_NAME ) + '-' + VersionInfo + BuildOpts + '-setup'


[Setup]
AppName={#AppName}
AppVerName={#AppName} {#VersionInfo}
AppPublisher={#Company}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
AppUpdatesURL={#AppURL}

; {pf}\{#AppName}
DefaultDirName={userappdata}\{#AppName}

DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
LicenseFile="..\..\runtime\license.txt"
InfoBeforeFile="..\..\README.TXT"
InfoAfterFile="..\..\runtime\version.txt"

OutputDir={#OutputDir}
OutputBaseFilename={#OutputBaseFilename}
OutputManifestFile="{#OutputBaseFilename}.manifest"
Compression=lzma/ultra
SolidCompression=yes

SetupIconFile="..\..\runtime\mmex.ico"
;WizardImageFile="..\..\graphics\new.bmp"
;WizardImageBackColor=clWhite
;WizardImageStretch=no

AppCopyright={#Copyright}
AppId=37153D93-6D91-4763-82BB-0DF646211ED0

VersionInfoVersion={#VersionInfo}
VersionInfoCompany={#Company}
VersionInfoDescription={#GetFileDescription( AppExePath )}
VersionInfoCopyright={#Copyright}

; disable the "Select Setup Language" dialog and have it rely solely on auto-detection
;ShowLanguageDialog=no


; "ArchitecturesAllowed=x64" specifies that Setup cannot run on anything but x64
#if CpuArch == "x86"
  ArchitecturesAllowed=x86 x64
#elif CpuArch == "amd64"
  ArchitecturesAllowed=x64
#else
  ArchitecturesAllowed={#CpuArch}
#endif

; "ArchitecturesInstallIn64BitMode=x64 ia64" requests that the install
; be done in "64-bit mode" on x64 & Itanium, meaning it should use the
; native 64-bit Program Files directory and the 64-bit view of the
; registry. On all other architectures it will install in "32-bit mode".
ArchitecturesInstallIn64BitMode=x64 ia64


[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"; InfoBeforeFile: "..\..\README.RU"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "he"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "no"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "eu"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "pt_br"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "ca"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "da"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "hu"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "sk"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "sl"; MessagesFile: "compiler:Languages\Slovenian.isl"


[Types]
Name: "full"; Description: "Full installation"
Name: "minimal"; Description: "Minimal installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "program"; Description: "Program Files"; Types: full minimal custom; Flags: fixed
Name: "help"; Description: "Help files"; Types: full minimal

Name: "lang"; Description: "Languages"; Types: full
Name: "lang\en"; Description: "English"; Types: full
Name: "lang\arabic"; Description: "Arabic"; Types: full
Name: "lang\bulgarian"; Description: "Bulgarian"; Types: full
Name: "lang\chinese_chs"; Description: "Chinese Chs"; Types: full
Name: "lang\chinese_zh"; Description: "Chinese Zh"; Types: full
Name: "lang\croatian"; Description: "Croatian"; Types: full
Name: "lang\cs"; Description: "Czech"; Types: full
Name: "lang\nl"; Description: "Dutch"; Types: full
Name: "lang\dutch_be"; Description: "Dutch Be"; Types: full
Name: "lang\english_uk"; Description: "English UK"; Types: full
Name: "lang\fr"; Description: "French"; Types: full
Name: "lang\de"; Description: "German"; Types: full
Name: "lang\greek"; Description: "Greek"; Types: full
Name: "lang\he"; Description: "Hebrew"; Types: full
Name: "lang\hu"; Description: "Hungarian"; Types: full
Name: "lang\indonesian"; Description: "Indonesian"; Types: full
Name: "lang\it"; Description: "Italian"; Types: full
Name: "lang\latvian"; Description: "Latvian"; Types: full
Name: "lang\no"; Description: "Norwegian"; Types: full
Name: "lang\pl"; Description: "Polish"; Types: full
Name: "lang\pt"; Description: "Portuguese"; Types: full
Name: "lang\portuguese_portugal"; Description: "Portuguese Portugal"; Types: full
Name: "lang\romanian"; Description: "Romanian"; Types: full
Name: "lang\ru"; Description: "Russian"; Types: full
Name: "lang\serbo_croatian"; Description: "Serbo Croatian"; Types: full
Name: "lang\sk"; Description: "Slovak"; Types: full
Name: "lang\sl"; Description: "Slovenian"; Types: full
Name: "lang\es"; Description: "Spanish"; Types: full
Name: "lang\swedish"; Description: "Swedish"; Types: full
Name: "lang\tamil"; Description: "Tamil"; Types: full
Name: "lang\turkish"; Description: "Turkish"; Types: full
Name: "lang\ukrainian"; Description: "Ukrainian"; Types: full
Name: "lang\vietnamese"; Description: "Vietnamese"; Types: full


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked


[Files]
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

#define lang_dir "{app}\en"

Source: "..\..\runtime\en\english.*";          DestDir: {#lang_dir}; Components: "lang\en";             Flags: ignoreversion
Source: "..\..\runtime\en\arabic.*";           DestDir: {#lang_dir}; Components: "lang\arabic";         Flags: ignoreversion
Source: "..\..\runtime\en\bulgarian.*";        DestDir: {#lang_dir}; Components: "lang\bulgarian";      Flags: ignoreversion
Source: "..\..\runtime\en\chinese_chs.*";      DestDir: {#lang_dir}; Components: "lang\chinese_chs";    Flags: ignoreversion
Source: "..\..\runtime\en\chinese_zh.*";       DestDir: {#lang_dir}; Components: "lang\chinese_zh";     Flags: ignoreversion
Source: "..\..\runtime\en\croatian.*";         DestDir: {#lang_dir}; Components: "lang\croatian";       Flags: ignoreversion
Source: "..\..\runtime\en\czech.*";            DestDir: {#lang_dir}; Components: "lang\cs";             Flags: ignoreversion
Source: "..\..\runtime\en\dutch.*";            DestDir: {#lang_dir}; Components: "lang\nl";             Flags: ignoreversion
Source: "..\..\runtime\en\dutch_be.*";         DestDir: {#lang_dir}; Components: "lang\dutch_be";       Flags: ignoreversion
Source: "..\..\runtime\en\english-uk.*";       DestDir: {#lang_dir}; Components: "lang\english_uk";     Flags: ignoreversion
Source: "..\..\runtime\en\french.*";           DestDir: {#lang_dir}; Components: "lang\fr";             Flags: ignoreversion
Source: "..\..\runtime\en\german.*";           DestDir: {#lang_dir}; Components: "lang\de";             Flags: ignoreversion
Source: "..\..\runtime\en\greek.*";            DestDir: {#lang_dir}; Components: "lang\greek";          Flags: ignoreversion
Source: "..\..\runtime\en\hebrew.*";           DestDir: {#lang_dir}; Components: "lang\he";             Flags: ignoreversion
Source: "..\..\runtime\en\hungarian.*";        DestDir: {#lang_dir}; Components: "lang\hu";             Flags: ignoreversion
Source: "..\..\runtime\en\indonesian.*";       DestDir: {#lang_dir}; Components: "lang\indonesian";     Flags: ignoreversion
Source: "..\..\runtime\en\italian.*";          DestDir: {#lang_dir}; Components: "lang\it";             Flags: ignoreversion
Source: "..\..\runtime\en\latvian.*";          DestDir: {#lang_dir}; Components: "lang\latvian";        Flags: ignoreversion
Source: "..\..\runtime\en\norwegian.*";        DestDir: {#lang_dir}; Components: "lang\no";             Flags: ignoreversion
Source: "..\..\runtime\en\polish.*";           DestDir: {#lang_dir}; Components: "lang\pl";             Flags: ignoreversion
Source: "..\..\runtime\en\portuguese.*";       DestDir: {#lang_dir}; Components: "lang\pt";             Flags: ignoreversion
Source: "..\..\runtime\en\portuguese_portugal.*"; DestDir: {#lang_dir}; Components: "lang\portuguese_portugal"; Flags: ignoreversion
Source: "..\..\runtime\en\romanian.*";         DestDir: {#lang_dir}; Components: "lang\romanian";       Flags: ignoreversion
Source: "..\..\runtime\en\russian.*";          DestDir: {#lang_dir}; Components: "lang\ru";             Flags: ignoreversion
Source: "..\..\runtime\en\serbo-croatian.*";   DestDir: {#lang_dir}; Components: "lang\serbo_croatian"; Flags: ignoreversion
Source: "..\..\runtime\en\slovak.*";           DestDir: {#lang_dir}; Components: "lang\sk";             Flags: ignoreversion
Source: "..\..\runtime\en\Slovenian.*";        DestDir: {#lang_dir}; Components: "lang\sl";             Flags: ignoreversion
Source: "..\..\runtime\en\spanish.*";          DestDir: {#lang_dir}; Components: "lang\es";             Flags: ignoreversion
Source: "..\..\runtime\en\swedish.*";          DestDir: {#lang_dir}; Components: "lang\swedish";       Flags: ignoreversion
Source: "..\..\runtime\en\tamil.*";            DestDir: {#lang_dir}; Components: "lang\tamil";         Flags: ignoreversion
Source: "..\..\runtime\en\turkish.*";          DestDir: {#lang_dir}; Components: "lang\turkish";       Flags: ignoreversion
Source: "..\..\runtime\en\ukrainian.*";        DestDir: {#lang_dir}; Components: "lang\ukrainian";     Flags: ignoreversion
Source: "..\..\runtime\en\vietnamese.*";       DestDir: {#lang_dir}; Components: "lang\vietnamese";    Flags: ignoreversion

#define help_dir "{app}\help"

Source: "..\..\docs\*.jpg"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\*.png"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\investment.html"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\index.html"; Languages: en; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\russian-help.html"; Languages: ru; DestName: "index.html"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\french-help.html";  Languages: fr; DestName: "index.html"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion
Source: "..\..\docs\index_fichiers\*";  Languages: fr; DestDir: "{#help_dir}\index_fichiers"; Components: help; Flags: ignoreversion
Source: "..\..\docs\italian-help.html"; Languages: it; DestName: "index.html"; DestDir: {#help_dir}; Components: help; Flags: ignoreversion

Source: "..\..\runtime\*"; DestDir: "{app}"; Components: program; Flags: ignoreversion
; .exe already compressed by UPX
Source: {#AppExePath}; DestDir: "{app}"; Components: program; Flags: ignoreversion nocompression
Source: {#CrtDlls}; DestDir: "{app}"; Components: program; Flags: ignoreversion

#if Format == "mingw"
  #define MingwDllPath  GetEnv("MINGWDIR") + "\bin\mingwm10.dll"
  Source: "{#MingwDllPath}"; DestDir: "{app}"; Components: program; Flags: ignoreversion
#endif


[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExeName}"; IconFilename: "{app}\{#AppExeName}"; WorkingDir: {app}; IconIndex: 0
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
; Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon
Name: "{userdesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon; IconFilename: "{app}\{#AppExeName}"; WorkingDir: {app}; IconIndex: 0
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: quicklaunchicon; WorkingDir: {app}; IconFilename: "{app}\{#AppExeName}"; IconIndex: 0

[Run]
Filename: "{app}\{#AppExeName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\mmexini.db3"
Type: files; Name: "{app}\*.png"
Type: files; Name: "{app}\*.txt"

[Messages]
en.WelcomeLabel1=Welcome to [name] Setup
ru.WelcomeLabel1=Вас приветствует инсталлятор [name]
