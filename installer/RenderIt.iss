; RenderIt installer definition for Inno Setup 6
#define AppVersion "0.6.0"
[Setup]
AppName=RenderIt
AppVersion={#AppVersion}
DefaultDirName={autopf}\RenderIt
DefaultGroupName=RenderIt
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=RenderIt_Setup
Compression=lzma2
SolidCompression=yes
PrivilegesRequired=admin

[Files]
Source: "..\dist\RenderIt\*"; DestDir: "{app}"; Flags: recursesubdirs ignoreversion

[Code]
procedure InstallForSketchUp(Version: String);
var
  Dst: String;
begin
  Dst := ExpandConstant('{userappdata}') + '\SketchUp\SketchUp ' + Version + '\SketchUp\Plugins\RenderIt';
  ForceDirectories(Dst);
  DirCopy(ExpandConstant('{app}'), Dst, False);
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    InstallForSketchUp('2025');
    InstallForSketchUp('2026');
  end;
end;
