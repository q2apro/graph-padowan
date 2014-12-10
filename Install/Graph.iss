#define GraphFileVer GetFileVersion("Graph.exe")
#define GraphProductVer GetStringFileInfo("Graph.exe", "ProductVersion")

[Setup]
AllowNoIcons=yes
AppCopyright=Ivan Johansen
AppMutex=Graph running
AppName=Graph
AppVerName=Graph {#GraphProductVer}
AppPublisher=Ivan Johansen
AppPublisherURL=http://www.padowan.dk
AppSupportURL=http://www.padowan.dk
AppUpdatesURL=http://www.padowan.dk
ChangesAssociations=Yes
Compression=lzma2
SolidCompression=Yes
DefaultDirName={pf}\Graph
DefaultGroupName=Graph
DisableProgramGroupPage=yes
InfoBeforeFile=License.txt
OutputBaseFilename=SetupGraphBeta-{#GraphFileVer}
OutputDir=.
UninstallDisplayIcon={app}\Graph.exe
VersionInfoVersion={#GraphFileVer}
VersionInfoProductVersion={#GraphProductVer}
PrivilegesRequired=None
;SignTool=SignTool /d $qGraph$q $f
SetupIconFile=Graph in a box.ico

[Languages]
Name: "Arabic"; MessagesFile: "compiler:Languages\Arabic.isl"
Name: "Basque"; MessagesFile: "compiler:Languages\Basque.isl"
Name: "Chinese_Simplified"; MessagesFile: "compiler:Languages\ChineseSimp.isl"
Name: "Chinese_Traditional"; MessagesFile: "compiler:Languages\ChineseTrad.isl"
Name: "Croatian"; MessagesFile:  "compiler:Languages\Croatian.isl"
Name: "Czech"; MessagesFile:  "compiler:Languages\Czech.isl"
Name: "Danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "Dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "English"; MessagesFile: "compiler:Default.isl"
Name: "Finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "French"; MessagesFile: "compiler:Languages\French.isl"
Name: "German"; MessagesFile: "compiler:Languages\German.isl"
Name: "Greek"; MessagesFile: "compiler:Languages\Greek.isl"
Name: "Hungarian"; MessagesFile: "compiler:Languages\Hungarian.isl"
Name: "Italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "Korean"; MessagesFile: "compiler:Languages\Korean.isl"
Name: "Mongolian"; MessagesFile: "compiler:Languages\Mongolian.isl"
Name: "Norwegian"; MessagesFile:  "compiler:Languages\Norwegian.isl"
Name: "Persian"; MessagesFile: "compiler:Languages\Farsi.isl"
Name: "Polish"; MessagesFile:  "compiler:Languages\Polish.isl"
Name: "Portuguese_Brazil"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "Portuguese_Portugal"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "Russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "Serbian"; MessagesFile: "compiler:Languages\Serbian.isl"
Name: "Slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "Spanish"; MessagesFile: "Languages\Spanish.isl"
Name: "Swedish"; MessagesFile:  "compiler:Languages\Swedish.isl"
Name: "Turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "Vietnamese"; MessagesFile: "compiler:Languages\Vietnamese.isl"

[Tasks]
Name: "DesktopIcon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "QuickLaunchIcon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "InstallAllUsers"; Description: "{cm:InstallAllUsers}"; Check: HasAdminRights

[Files]
Source: "Graph.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "PDFlib.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "Thumbnails.dll"; DestDir: "{app}"; Flags: regserver noregerror restartreplace uninsrestartdelete;
;Source: "Thumbnails32.dll"; DestDir: "{app}"; DestName: "Thumbnails.dll"; Flags: regserver noregerror restartreplace uninsrestartdelete 32bit;
;Source: "Thumbnails64.dll"; DestDir: "{app}"; DestName: "Thumbnails.dll"; Flags: regserver noregerror restartreplace uninsrestartdelete 64bit;
Source: "License.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "Examples\*.grf"; DestDir: "{app}\Examples"; Flags: ignoreversion
Source: "Help\*.chm"; DestDir: "{app}\Help"; Flags: ignoreversion
Source: "Locale\*.mo"; DestDir: "{app}\Locale"; Flags: ignoreversion overwritereadonly
Source: "Plugins\*.py"; DestDir: "{app}\Plugins"; Flags: ignoreversion overwritereadonly
Source: "Plugins\*.bmp"; DestDir: "{app}\Plugins"; Flags: ignoreversion overwritereadonly
Source: "Lib\*.py"; DestDir: "{app}\Lib"; Flags: ignoreversion overwritereadonly

[Icons]
Name: "{userprograms}\Graph\{cm:Examples}"; Filename: "{app}\Examples"; IconFilename: "{sys}\shell32.dll"; IconIndex: 19; Flags: foldershortcut; Tasks: not InstallAllUsers; Check: not WizardNoIcons
Name: "{userprograms}\Graph\Graph"; Filename: "{app}\Graph.exe"; Comment: "Graph"; Tasks: not InstallAllUsers; Check: not WizardNoIcons
Name: "{userprograms}\Graph\{cm:Uninstall}"; Filename: "{uninstallexe}"; Comment: "Uninstall Graph"; Check: not WizardNoIcons
Name: "{commonprograms}\Graph\{cm:Examples}"; Filename: "{app}\Examples"; Flags: foldershortcut; Tasks: InstallAllUsers; Check: not WizardNoIcons
Name: "{commonprograms}\Graph\Graph"; Filename: "{app}\Graph.exe"; Comment: "Graph"; Tasks: InstallAllUsers; Check: not WizardNoIcons
Name: "{userdesktop}\Graph"; Filename: "{app}\Graph.exe"; Tasks: DesktopIcon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\Graph"; Filename: "{app}\Graph.exe"; Tasks: QuickLaunchIcon

[Run]
Filename: "{app}\Graph.exe"; Parameters: "/regserver"
Filename: "{app}\Graph.exe"; Description: "{cm:LaunchProgram,Graph}"; Flags: nowait postinstall skipifsilent unchecked

[Registry]
Root: HKCU; Subkey: "Software\Classes\.grf"; ValueType: string; ValueName: ""; ValueData: "GraphFile"; Flags: uninsdeletevalue noerror; Tasks: not InstallAllUsers
Root: HKLM; Subkey: "Software\Classes\.grf"; ValueType: string; ValueName: ""; ValueData: "GraphFile"; Flags: uninsdeletevalue noerror; Tasks: InstallAllUsers
Root: HKCU; Subkey: "Software\Ivan\Graph"; ValueType: string; ValueName: "Language"; ValueData: "{code:GetLang}"; Flags: noerror
Root: HKLM; Subkey: "Software\Ivan\Graph"; ValueType: string; ValueName: "Language"; ValueData: "{code:GetLang}"; Flags: noerror; Tasks: InstallAllUsers
Root: HKCU; Subkey: "Software\Ivan\Graph"; Flags: uninsdeletekey noerror
Root: HKLM; Subkey: "Software\Ivan\Graph"; Flags: uninsdeletekey noerror
Root: HKLM; SubKey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\Graph"; Flags: deletekey
Root: HKCU; Subkey: "Software\Ivan\Graph"; ValueType: dword; ValueName: "InstallAllUsers"; ValueData: "0"; Flags: noerror; Tasks: not InstallAllUsers
Root: HKCU; Subkey: "Software\Ivan\Graph"; ValueType: dword; ValueName: "InstallAllUsers"; ValueData: "1"; Flags: noerror; Tasks: InstallAllUsers

[InstallDelete]
Type: files; Name: "{app}\uninstall.exe"
Type: files; Name: "{app}\uninstall.ini"
Type: files; Name: "{app}\Help\*.gid"
Type: files; Name: "{app}\Help\*.hlp"
Type: files; Name: "{app}\Help\*.cnt"
Type: files; Name: "{userprograms}\Graph.lnk"
Type: files; Name: "{commonprograms}\Graph.lnk"
Type: files; Name: "{app}\Locale\Chinese.mo"
Type: files; Name: "{app}\Locale\Portuguese.mo"
Type: files; Name: "{userprograms}\Graph\Uninstall.lnk"
Type: files; Name: "{commonprograms}\Graph\Uninstall.lnk"
Type: filesandordirs; Name: "{userprograms}\Graph\Examples"
Type: filesandordirs; Name: "{commonprograms}\Graph\Examples"

[UninstallRun]
Filename: "{app}\Graph.exe"; Parameters: "/unregserver"

[UninstallDelete]
Type: files; Name: "{app}\Graph.log"
Type: files; Name: "{app}\PyGraph.log"
Type: files; Name: "{app}\Graph.err"
Type: files; Name: "{app}\Thumbnails.log"
Type: files; Name: "{app}\Thumbnails.dll"
Type: files; Name: "{app}\Examples\Thumbs.db"
Type: files; Name: "{app}\Plugins\*.pyc"
Type: files; Name: "{app}\Lib\*.pyc"
Type: filesandordirs; Name: "{app}\Plugins\__pycache__"
Type: filesandordirs; Name: "{app}\Lib\__pycache__"
Type: dirifempty; Name: "{app}\Examples"
Type: dirifempty; Name: "{app}\Help"
Type: dirifempty; Name: "{app}\Locale"
Type: dirifempty; Name: "{app}\Plugins"
Type: dirifempty; Name: "{app}"

[Messages]
English.WelcomeLabel1=Welcome to the [name/ver] Setup Wizard

[CustomMessages]
InstallAllUsers=Install for all users
Basque.InstallAllUsers=Instalatu erabiltzaile guztientzat
Danish.InstallAllUsers=Installér for alle brugere
Dutch.InstallAllUsers=Installeer voor alle gebruikers
French.InstallAllUsers=Installer pour tous les utilisateurs
German.InstallAllUsers=Für alle Benutzer installieren
Greek.InstallAllUsers=Εγκατάσταση για όλους τους χρήστες
Hungarian.InstallAllUsers=Telepítés minden felhasználónak
Italian.InstallAllUsers=Installa per tutti gli utenti
Norwegian.InstallAllUsers=Installer for alle brukere
Persian.InstallAllUsers=نصب براي همه کاربران
Portuguese_Brazil.InstallAllUsers=Instala para todos os usuários
Portuguese_Portugal.InstallAllUsers=Instala para todos os usuários
Russian.InstallAllUsers=Установить для всех пользователей
Slovenian.InstallAllUsers=Namesti za vse uporabnike
Spanish.InstallAllUsers=Instalar para todos los usuarios
Swedish.InstallAllUsers=Installera för alla användare

Examples=Examples
Basque.Examples=Adibideak
Croatian.Examples=Primjeri
Danish.Examples=Eksempler
Dutch.Examples=Voorbeelden
French.Examples=Exemples
German.Examples=Beispiele
Hungarian.Examples=Példák
Norwegian.Examples=Eksempler
Portuguese_Portugal.Examples=Exemplos
Spanish.Examples=Ejemplos
Swedish.Examples=Exempel

Uninstall=Uninstall
Arabic.Uninstall=إلغاء تثبيت
Basque.Uninstall=Desinstalaketa
Chinese_Simplified.Uninstall=安装程序
Chinese_Traditional.Uninstall=安裝程式
Croatian.Uninstall=Deinstalacija
Czech.Uninstall=Prùvodce odinstalací
Danish.Uninstall=Afinstaller
Dutch.Uninstall=Verwijderen
Finnish.Uninstall=Asennuksen poisto
French.Uninstall=Désinstallation
German.Uninstall=Entfernen
Greek.Uninstall=Απεγκατάσταση
Hungarian.Uninstall=Eltávolító
Italian.Uninstall=Disinstallazione
Korean.Uninstall=프로그램 제거
Mongolian.Uninstall=Óñòãàõ
Norwegian.Uninstall=Avinstaller
Persian.Uninstall=حذف برنامه
Polish.Uninstall=Deinstalacja
Portuguese_Brazil.Uninstall=Desinstalar
Portuguese_Portugal.Uninstall=Desinstalação
Russian.Uninstall=Деинсталляция
Serbian.Uninstall=Deinstalacija
Slovenian.Uninstall=Odstranitev
Spanish.Uninstall=Desinstalar
Swedish.Uninstall=Avinstallation
Turkish.Uninstall=Kaldýr
Vietnamese.Uninstall=GõÞ boÒ

[code]
function FilesEqual(FileName : String): Boolean;
var
  SourceFile, DestFile, MD5OfDestFile, MD5OfSourceFile : String;
begin
  Result := False;
  DestFile := ExpandConstant('{app}\' + FileName);
  SourceFile := ExpandConstant('{tmp}\' + FileName);
  if not FileExists(DestFile) then
  begin
    Log('File ' + DestFile + ' does not exist');
    Exit;
  end;

  if not FileExists(SourceFile) then
    ExtractTemporaryFile(FileName);
  MD5OfSourceFile := GetMD5OfFile(SourceFile);
  MD5OfDestFile := GetMD5Offile(DestFile);
  Log('MD5 of ' + SourceFile + ': ' + MD5OfSourceFile);
  Log('MD5 of ' + DestFile + ': ' + MD5OfDestFile);
  Result := MD5OfDestFile = MD5OfSourceFile;
  if Result then Log('Files are equal');
end;

function HasAdminRights : Boolean;
begin
  Result := RegWriteStringValue(HKEY_LOCAL_MACHINE, 'Software\Classes\GraphTest', '', '');
  RegDeleteKeyIncludingSubkeys(HKEY_LOCAL_MACHINE, 'Software\Classes\GraphTest');
end;

function GetLang(S:string):string;
var
  I : Byte;
begin
  Result := ActiveLanguage();
  I := Pos('_', Result);
  if I <> 0 then
    Result := Copy(Result, 1, I-1) + ' (' + Copy(Result, I+1, MaxInt) + ')';
end;







