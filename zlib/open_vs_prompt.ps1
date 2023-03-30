$Arch = "x86"
$VSVersion = "2022"


Write-Host "Arch is: $Arch VSVersion is: $VSVersion"


$registryPath = "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\App Paths\devenv.exe"

try {
    $devenvPath = (Get-ItemProperty -Path $registryPath).'(default)'.Trim('"')
} catch {
    Write-Host "Visual Studio $VSVersion not found. Please make sure it is installed." -ForegroundColor Red
    pause
    exit 1
}

$vsInstallationPath = (Split-Path (Split-Path (Split-Path $devenvPath) -Parent) -Parent)
Write-Host "The Visual Studio installation path is: $vsInstallationPath"

$vsInstallationVersion = ($vsInstallationPath -split '\\')[-2]


Write-Host "Detected Visual Studio installation version: $vsInstallationVersion"
Write-Host "Using the provided VSVersion parameter: $VSVersion"

$vsInstallationPath = $vsInstallationPath -replace $vsInstallationVersion, $VSVersion


if (-not (Test-Path $vsInstallationPath)) {
    Write-Host "The Visual Studio installation path does not exist. Please check the provided version and make sure it is installed." -ForegroundColor Red
    pause
    exit 1
}

$devCmdPath = Join-Path $vsInstallationPath "Common7\Tools\VsDevCmd.bat"
Write-Host "Prompt path is: $devCmdPath"

if (-not (Test-Path $devCmdPath)) {
    Write-Host "VsDevCmd.bat not found in the Visual Studio installation path." -ForegroundColor Red
    pause
    exit 1
}

cmd.exe /k "`"$devCmdPath`" -arch=$Arch"
