; *** Inno Setup version 5.1.0+ Spanish messages ***
;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: When translating this text, do not add periods (.) to the end of
; messages that didn't have them already, because on those messages Inno
; Setup adds the periods automatically (appending a period would result in
; two periods being displayed).
;
; $jrsoftware: issrc/Files/Default.isl,v 1.66 2005/02/25 20:23:48 mlaan Exp $
; Translated by Emiliano Llano Díaz and Francisco Oliver
; emiliano_d@hotmail.com

[LangOptions]
LanguageName=Espa<00f1>ol
LanguageID=$0c0a
LanguageCodePage=0
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=verdana
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Instalación de Graph
SetupWindowTitle=Instalación de Graph
UninstallAppTitle=Desinstalación de Graph
UninstallAppFullTitle=Desinstalación de Graph

; *** Misc. common
InformationTitle=Información
ConfirmTitle=Confirmar
ErrorTitle=Error

; *** SetupLdr messages
SetupLdrStartupMessage=Este Asistente instalará %1 en su ordenador. ¿Desea continuar?
LdrCannotCreateTemp=Imposible crear archivo temporal. Instalación cancelada
LdrCannotExecTemp=Imposible ejecutar archivo en el directorio temporal. Instalación abortada

; *** Startup error messages
LastErrorMessage=%1.%n%nError %2: %3
SetupFileMissing=El archivo %1 no se encuentra en el directorio de instalación. Por favor, resuelva el problema u obtenga una nueva copia del programa.
SetupFileCorrupt=Los archivos de instalación están dañados. Por favor, obtenga una nueva copia del programa.
SetupFileCorruptOrWrongVer=Los archivos de instalación están dañados, o son incompatibles con esta versión del Asistente. Por favor, resuelva el problema u obtenga una nueva copia del programa.
NotOnThisPlatform=Este Asistente no se ejecutará en %1.
OnlyOnThisPlatform=Este Asistente debe ser ejecutado en %1.
OnlyOnTheseArchitectures=Este programa sólo puede ser instalado en versiones de Windows diseñadas para las siguientes arquitecturas del procesador:%n%n%1
MissingWOW64APIs=La versión de Windows que está ejecutando no tiene la funcionalidad exigida por el Asistente para realizar una instalación de 64 bits. Para resolver este problema, por favor, instale Service Pack %1.
WinVersionTooLowError=Este programa necesita %1 versión %2 o posterior.
WinVersionTooHighError=Este programa no puede ser instalado en %1 versión %2 o posterior.
AdminPrivilegesRequired=Debe estar registrado como administrador para instalar este programa.
PowerUserPrivilegesRequired=Debe estar registrado como administrador, o como miembro del Grupo de Usuarios Avanzados, para poder instalar este programa.
SetupAppRunningError=El Asistente de instalación ha detectado que %1 está en ejecución.%n%nPor favor, cierre el programa y haga clic en Aceptar para continuar, o en Cancelar para salir.
UninstallAppRunningError=El Asistente de desinstalación ha detectado que %1 está en ejecución.%n%nPor favor, cierre el programa y haga clic en Aceptar para continuar, o en Cancelar para salir.

; *** Misc. errors
ErrorCreatingDir=El Asistente no ha podido crear el directorio "%1"
ErrorTooManyFilesInDir=Imposible crear un archivo en el directorio "%1" porque éste contiene demasiados archivos

; *** Setup common messages
ExitSetupTitle=Instalación de Graph
ExitSetupMessage=La instalación ha sido detenida. Si abandona ahora, el programa no se instalará.%n%nPodrá ejecutar el Asistente en otro momento, y completar la instalación.%n%n¿Desea salir del Asistente?
AboutSetupMenuItem=&Acerca de la instalación...
AboutSetupTitle=Acerca de la instalación
AboutSetupMessage=%1 versión %2%n%3%n%n%1 página Web:%n%4
AboutSetupNote=
TranslatorNote=


; *** Buttons
ButtonBack=< &Atrás
ButtonNext=&Siguiente >
ButtonInstall=&Instalar
ButtonOK=Aceptar
ButtonCancel=Cancelar
ButtonYes=&Sí
ButtonYesToAll=Sí a &todo
ButtonNo=&No
ButtonNoToAll=N&o a todo
ButtonFinish=&Finalizar
ButtonBrowse=&Examinar...
ButtonWizardBrowse=E&xaminar...
ButtonNewFolder=&Crear nueva carpeta


; *** "Select Language" dialog messages
SelectLanguageTitle=Selección de idioma
SelectLanguageLabel=Seleccione el idioma que desea utilizar durante la instalación de Graph:

; *** Common wizard text
ClickNext=Haga clic en Siguiente para continuar, o en Cancelar para salir del Asistente.
BeveledLabel=
BrowseDialogTitle=Buscar carpeta
BrowseDialogLabel=Seleccione una carpeta de la lista inferior, y haga clic en Aceptar.
NewFolderName=Nueva carpeta

; *** "Welcome" wizard page
WelcomeLabel1=Bienvenido al Asistente de instalación de [name]
WelcomeLabel2=Este Asistente instalará [name/ver] en su ordenador.%n%nSe recomienda cerrar todas las aplicaciones en ejecución antes de continuar.

; *** "Password" wizard page
WizardPassword=Contraseña
PasswordLabel1=Esta instalación está protegida por contraseña.
PasswordLabel3=Por favor, indique la contraseña, y haga clic en Continuar.%n%nLas contraseñas distinguen mayúsculas y minúsculas.
PasswordEditLabel=&Contraseña:
IncorrectPassword=La contraseña introducida no es correcta. Por favor, inténtelo de nuevo.

; *** "License Agreement" wizard page
WizardLicense=Acuerdo de licencia
LicenseLabel=Por favor, lea la siguiente información importante antes de continuar.
LicenseLabel3=Por favor, lea el siguiente Acuerdo de licencia. Debe aceptar los términos de este Acuerdo antes de proseguir con la instalación.
LicenseAccepted=&Acepto el Acuerdo de licencia
LicenseNotAccepted=&No acepto el Acuerdo de licencia

; *** "Information" wizard pages
WizardInfoBefore=Información
InfoBeforeLabel=Por favor, lea la siguiente información importante antes de continuar.
InfoBeforeClickLabel=Cuando esté listo para continuar con la instalación, haga clic en Siguiente.
WizardInfoAfter=Información
InfoAfterLabel=Por favor, lea la siguiente información importante antes de continuar.
InfoAfterClickLabel=Cuando esté listo para continuar con la instalación, haga clic en Siguiente.

; *** "User Information" wizard page
WizardUserInfo=Información del usuario
UserInfoDesc=Por favor, introduzca su información.
UserInfoName=Nombre del &usuario:
UserInfoOrg=&Organización:
UserInfoSerial=Número de &serie:
UserInfoNameRequired=Debe introducir un nombre.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Seleccione directorio de destino
SelectDirDesc=¿Dónde se instalará [name]?
SelectDirLabel3=El Asistente instalará [name] en el directorio indicado.
SelectDirBrowseLabel=Para continuar, haga clic en Siguiente. Si desea seleccionar un directorio diferente, haga clic en Examinar.
DiskSpaceMBLabel=Se necesitan, al menos, [mb] MB de espacio libre en el disco duro.
ToUNCPathname=El Asistente no puede realizar la instalación en un directorio UNC. Si está tratando de instalar el programa en una red, necesitará asignarlo a una unidad de la red.
InvalidPath=Debe introducir una ruta completa con la letra de la unidad; por ejemplo:%nC:\APP%n%no un directorio UNC en la forma:%n%n\\server\share
InvalidDrive=La unidad o el directorio UNC que ha seleccionado no existe. Por favor, seleccione otra.
DiskSpaceWarningTitle=No hay espacio suficiente en el disco duro
DiskSpaceWarning=El Asistente necesita, al menos, %1 KB de espacio libre para la instalación, pero la unidad seleccionada sólo tiene %2 KB disponibles.%n%n¿Desea continuar de todos modos?
DirNameTooLong=El nombre de la carpeta o ruta es demasiado largo.
InvalidDirName=El nombre de la carpeta no es válido.
BadDirName32=El nombre de la carpeta no puede incluir los siguientes caracteres:%n%n%1
DirExistsTitle=La carpeta ya existe
DirExists=La carpeta:%n%n%1%n%nya existe. ¿Desea realizar la instalación en esta carpeta de todos modos?
DirDoesntExistTitle=La carpeta no existe
DirDoesntExist=La carpeta:%n%n%1%n%nno existe. ¿Desea crear esa carpeta?

; *** "Select Components" wizard page
WizardSelectComponents=Selección de componentes
SelectComponentsDesc=¿Qué componentes desea instalar?
SelectComponentsLabel2=Seleccione los componentes que desea instalar, y deseleccione aquellos que no desea instalar. Cuando esté listo para continuar, haga clic en Siguiente.
FullInstallation=Instalación completa
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalación compacta
CustomInstallation=Instalación personalizada
NoUninstallWarningTitle=Componentes existentes
NoUninstallWarning=El Asistente ha detectado que los siguientes componentes ya están instalados en su ordenador:%n%n%1%n%nLa deselección de esos componentes no significa su desinstalación.%n%n¿Desea continuar de todos modos?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=La selección actual necesita, al menos, [mb] MB de espacio libre en el disco duro.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Seleccione opciones adicionales de instalación
SelectTasksDesc=¿Qué tareas adicionales deben ser ejecutadas?
SelectTasksLabel2=Seleccione las opciones que desea que el Asistente ejecute mientras instala [name], y haga clic en Siguiente.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Seleccione la carpeta del menú Inicio
SelectStartMenuFolderDesc=¿Dónde deben colocarse los accesos directos del programa?
SelectStartMenuFolderLabel3=El Asistente creará los accesos directos del programa en la siguiente carpeta del menú Inicio.
SelectStartMenuFolderBrowseLabel=Para continuar, haga clic en Siguiente. Si desea seleccionar una carpeta diferente, haga clic en Examinar.
MustEnterGroupName=Debe introducir un nombre de carpeta.
GroupNameTooLong=El nombre de la carpeta o ruta es demasiado largo.
InvalidGroupName=El nombre de la carpeta no es válido.
BadGroupName=El nombre de la carpeta no puede incluir ninguno de los siguientes caracteres:%n%n%1
NoProgramGroupCheck2=&No crear una carpeta en el menú Inicio

; *** "Ready to Install" wizard page
WizardReady=Preparado para instalar
ReadyLabel1=El Asistente está listo para iniciar la instalación de [name] en su ordenador.
ReadyLabel2a=Haga clic en Instalar para comenzar la instalación, o en Atrás si desea revisar o cambiar la configuración.
ReadyLabel2b=Haga clic en Instalar para comenzar la instalación.
ReadyMemoUserInfo=Información del usuario:
ReadyMemoDir=Directorio de instalación:
ReadyMemoType=Tipo de instalación:
ReadyMemoComponents=Componentes seleccionados:
ReadyMemoGroup=Carpeta del menú Inicio:
ReadyMemoTasks=Opciones adicionales:

; *** "Preparing to Install" wizard page
WizardPreparing=Preparándose para instalar
PreparingDesc=El Asistente está preparándose para instalar [name] en su ordenador.
PreviousInstallNotCompleted=La instalación/desinstalación de una versión previa del programa no se completó. Debe reiniciar el ordenador para completar la instalación.%n%nTras reiniciar el ordenador, ejecute de nuevo el Asistente para llevar a cabo la instalación de [name].
CannotContinue=El Asistente no puede continuar. Por favor, haga clic en Cancelar para salir.

; *** "Installing" wizard page
WizardInstalling=Instalando...
InstallingLabel=Por favor, espere mientras el Asistente instala [name] en su ordenador.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Finalizando la instalación de [name]
FinishedLabelNoIcons=El Asistente ha concluido la instalación de [name] en su ordenador.
FinishedLabel=El Asistente ha terminado de instalar [name] en su ordenador. Puede abrir el programa con los accesos directos creados.
ClickFinish=Haga clic en Finalizar para concluir el proceso de instalación.
FinishedRestartLabel=Para completar la instalación de [name] debe reiniciar su ordenador. ¿Desea reiniciar ahora?
FinishedRestartMessage=Para completar la instalación de [name] debe reiniciar su ordenador.%n%n¿Desea reiniciar ahora?
ShowReadmeCheck=Sí, deseo ver el archivo LÉAME
YesRadio=&Sí, deseo reiniciar el ordenador ahora
NoRadio=&No, reiniciaré el ordenador más tarde
; used for example as 'Run MyProg.exe'
RunEntryExec=Ejecutar %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Ver %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=El Asistente necesita el siguiente disco
SelectDiskLabel2=Por favor, inserte el disco %1 y haga clic en Aceptar.%n%nSi los archivos se hallan en una carpeta diferente a la mostrada abajo, indique la ruta correcta o haga clic en Examinar.
PathLabel=&Ruta:
FileNotInDir2=El archivo "%1" no se encuentra en "%2". Por favor, inserte el disco correcto o seleccione otra carpeta.
SelectDirectoryLabel=Por favor, especifique la ubicación del siguiente disco.

; *** Installation phase messages
SetupAborted=La instalación no ha sido completada.%n%nPor favor, resuelva el problema y ejecute de nuevo el Asistente de instalación.
EntryAbortRetryIgnore=Haga clic en Reintentar para intentar de nuevo, en Ignorar para proceder de cualquier forma, o en Cancelar para interrumpir la instalación.

; *** Installation status messages
StatusCreateDirs=Creando carpetas...
StatusExtractFiles=Extrayendo archivos...
StatusCreateIcons=Creando accesos directos...
StatusCreateIniEntries=Creando entradas INI...
StatusCreateRegistryEntries=Creando entradas en el Registro...
StatusRegisterFiles=Registrando archivos...
StatusSavingUninstall=Guardando información para desinstalar el programa...
StatusRunProgram=Finalizando la instalación...
StatusRollback=Deshaciendo los cambios...

; *** Misc. errors
ErrorInternal2=Error Interno %1
ErrorFunctionFailedNoCode=%1 falló
ErrorFunctionFailed=%1 falló; código %2
ErrorFunctionFailedWithMessage=%1 falló; código %2.%n%3
ErrorExecutingProgram=Imposible ejecutar el archivo:%n%1

; *** Registry errors
ErrorRegOpenKey=Error abriendo la clave de registro:%n%1\%2
ErrorRegCreateKey=Error al crear la clave de registro:%n%1\%2
ErrorRegWriteKey=Error al escribir la clave de registro:%n%1\%2

; *** INI errors
ErrorIniEntry=Error al crear entrada INI en el archivo "%1".

; *** File copying errors
FileAbortRetryIgnore=Haga clic en Reintentar para probar de nuevo, en Ignorar para omitir este archivo (no recomendado), o en Cancelar para interrumpir la instalación.
FileAbortRetryIgnore2=Haga clic en Reintentar para probar de nuevo, en Ignorar para proceder de cualquier forma (no recomendado), o en Cancelar para interrumpir la instalación.
SourceIsCorrupted=El archivo de origen está dañado
SourceDoesntExist=El archivo de origen "%1" no existe
ExistingFileReadOnly=El archivo existente está marcado como de sólo lectura.%n%nHaga clic en Reintentar para quitar el atributo de sólo lectura y probar de nuevo, en Ignorar para omitir este archivo, o en Cancelar para interrumpir la instalación.
ErrorReadingExistingDest=Ocurrió un error al tratar de leer el archivo:
FileExists=El archivo ya existe.%n%n¿Desea sobrescribirlo?
ExistingFileNewer=El archivo existente es más reciente que el que está tratando de instalar. Se recomienda conservarlo.%n%n¿Desea mantener el archivo existente?
ErrorChangingAttr=Ocurrió un error al tratar de cambiar los atributos del archivo:
ErrorCreatingTemp=Ocurrió un error al tratar de crear un archivo en la carpeta de destino:
ErrorReadingSource=Ocurrió un error al tratar de leer el archivo de origen:
ErrorCopying=Ocurrió un error al tratar de copiar el archivo:
ErrorReplacingExistingFile=Ocurrió un error al tratar de reemplazar el archivo existente:
ErrorRestartReplace=Falló el reintento de reemplazar:
ErrorRenamingTemp=Ocurrió un error al tratar de renombrar un archivo en la carpeta de destino:
ErrorRegisterServer=Imposible registrar la biblioteca DLL/OCX: %1
ErrorRegisterServerMissingExport=No se encontró DllRegisterServer export
ErrorRegisterTypeLib=Imposible registrar la biblioteca de tipo: %1

; *** Post-installation errors
ErrorOpeningReadme=Ocurrió un error al tratar de abrir el archivo LÉAME.
ErrorRestartingComputer=El Asistente no ha podido reiniciar el ordenador. Por favor, hágalo manualmente.

; *** Uninstaller messages
UninstallNotFound=El archivo "%1" no existe. No se puede desinstalar
UninstallOpenError=El archivo "%1" no puede ser abierto. No se puede desinstalar
UninstallUnsupportedVer=El archivo de registro para desinstalar "%1" se halla en un formato no reconocido por esta versión del programa. No se puede desinstalar
UninstallUnknownEntry=Una entrada desconocida (%1) se encontró en el registro de desinstalación
ConfirmUninstall=¿Está seguro que desea eliminar completamente %1 y todos sus componentes?
UninstallOnlyOnWin64=Esta instalación sólo puede ser desinstalada en un sistema operativo Windows de 64 bits.
OnlyAdminCanUninstall=Este programa sólo puede desinstalarlo un usuario con privilegios de administrador.
UninstallStatusLabel=Por favor, espere mientras %1 se desinstala de su ordenador.
UninstalledAll=%1 se desinstaló con éxito de su ordenador.
UninstalledMost=Finalizó la desinstalación de %1.%n%nAlgunos elementos no pudieron ser eliminados, pero puedo hacerlo manualmente.
UninstalledAndNeedsRestart=Para completar la desinstalación de %1 se requiere reiniciar el ordenador.%n%n¿Desea reiniciarlo ahora?
UninstallDataCorrupted=El archivo "%1" está dañado. No se puede desinstalar

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=¿Eliminar archivo compartido?
ConfirmDeleteSharedFile2=El sistema informa que el siguiente archivo compartido no es utilizado por ningún otro programa. ¿Desea eliminar este archivo?%n%nSi otros programas están usándolo y es eliminado, éstos podrían no funcionar correctamente. Si no está seguro, elija No. Dejar dicho archivo en su sistema no causa ningún daño.
SharedFileNameLabel=Nombre del archivo:
SharedFileLocationLabel=Ubicación:
WizardUninstalling=Estado de la desinstalación
StatusUninstalling=Desinstalando %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 versión %2
AdditionalIcons=Iconos adicionales:
CreateDesktopIcon=Crear un acceso directo en el &Escritorio
CreateQuickLaunchIcon=Crear un acceso directo en la barra de Inicio &rápido
ProgramOnTheWeb=%1 en la Web
UninstallProgram=Desinstalar %1
LaunchProgram=Ejecutar %1
AssocFileExtension=&Asociar %1 con la extensión de archivo %2
AssocingFileExtension=Asociar %1 con la extensión de archivo %2...
