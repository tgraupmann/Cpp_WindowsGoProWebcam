[Setup]
AppName=GoPro Webcam Mode Auto Switcher
AppVerName=GoPro Webcam Mode Auto Switcher 1.0
AppPublisher=Tim Graupmann
AppPublisherURL=https://github.com/tgraupmann/Cpp_WindowsGoProWebcam
AppSupportURL=https://github.com/tgraupmann/Cpp_WindowsGoProWebcam
AppUpdatesURL=https://github.com/tgraupmann/Cpp_WindowsGoProWebcam
DefaultDirName={userappdata}\GoProWebcamModeAutoSwitcher
DefaultGroupName=GoProWebcamModeAutoSwitcher
OutputBaseFilename=GoProWebcamModeAutoSwitcherSetup
SetupIconFile=release_icon.ico
UninstallDisplayIcon=release_icon.ico
Compression=lzma
SolidCompression=yes
InfoBeforeFile=LICENSE
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "Release\Cpp_WindowsGoProWebcam.exe"; DestDir: "{userappdata}\GoProWebcamModeAutoSwitcher"; CopyMode: alwaysoverwrite;

[Icons]
Name: "{group}\GoProWebcamModeAutoSwitcher"; Filename: "{userappdata}\GoProWebcamModeAutoSwitcher\Cpp_WindowsGoProWebcam.exe"; WorkingDir: "{app}";
Name: "{commondesktop}\GoProWebcamModeAutoSwitcher"; Filename: "{userappdata}\GoProWebcamModeAutoSwitcher\Cpp_WindowsGoProWebcam.exe"; WorkingDir: "{app}";
Name: "{group}\Uninstall GoProWebcamModeAutoSwitcher"; Filename: "{uninstallexe}"

[Run]
Filename: "{userappdata}\GoProWebcamModeAutoSwitcher\Cpp_WindowsGoProWebcam.exe"; Description: "Launch GoPro Webcam Mode Auto Switcher"; Flags: postinstall skipifsilent runascurrentuser; WorkingDir: "{app}"
