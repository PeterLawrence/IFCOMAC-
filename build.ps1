# PowerShell CMake Build Helper for IfcSpacialImport
# Usage: .\build.ps1 -Configuration Debug
#        .\build.ps1 -Configuration Release
#        .\build.ps1 -CleanAll

param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",
    
    [switch]$Clean,
    
    [switch]$CleanAll,
    
    [string]$CMakeGenerator = "Visual Studio 15 2017",
    
    [ValidateSet("x64", "Win32")]
    [string]$Platform = "x64",
    
    [string]$IfcOpenShellRoot = "E:/GitProjects/BIM-IFC/IfcOpenshell"
)

# Colors for output
$ErrorColor = "Red"
$SuccessColor = "Green"
$InfoColor = "Cyan"

function Write-Info {
    param([string]$Message)
    Write-Host $Message -ForegroundColor $InfoColor
}

function Write-Success {
    param([string]$Message)
    Write-Host $Message -ForegroundColor $SuccessColor
}

function Write-Error-Custom {
    param([string]$Message)
    Write-Host $Message -ForegroundColor $ErrorColor
}

Write-Host ""
Write-Info "==============================================="
Write-Info "IfcSpacialImport CMake Build Helper"
Write-Info "==============================================="
Write-Info "Configuration: $Configuration"
Write-Info "Generator: $CMakeGenerator"
Write-Info "Platform: $Platform"
Write-Info "IfcOpenShell Root: $IfcOpenShellRoot"
Write-Host ""

# Handle CleanAll
if ($CleanAll) {
    Write-Info "Removing build directory..."
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build" -ErrorAction SilentlyContinue
        Write-Success "Build directory removed."
    }
    Write-Info "Run this script again to configure and build."
    exit 0
}

# Handle Clean
if ($Clean) {
    Write-Info "Cleaning build artifacts..."
    if (Test-Path "build") {
        Set-Location build
        & cmake --build . --config $Configuration --target clean 2>$null
        Set-Location ..
        Write-Success "Clean complete."
    }
    exit 0
}

# Ensure build directory exists
if (-not (Test-Path "build")) {
    Write-Info "Creating build directory..."
    New-Item -ItemType Directory -Path "build" -Force | Out-Null
}

# Configure
Write-Host ""
Write-Info "Configuring with CMake..."
Set-Location build
& cmake .. `
    -G $CMakeGenerator `
    -A $Platform `
    -DIFCOPENSHELL_ROOT=$IfcOpenShellRoot `
    2>&1 | Tee-Object -Variable ConfigOutput | Out-Null

if ($LASTEXITCODE -ne 0) {
    Write-Error-Custom "Configuration failed!"
    Write-Host $ConfigOutput
    exit 1
}

Set-Location ..
Write-Success "Configuration complete."
Write-Host ""

# Build
Write-Info "Building $Configuration configuration..."
$BuildOutput = & cmake --build build --config $Configuration 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Error-Custom "Build failed!"
    Write-Host $BuildOutput
    exit 1
}

Write-Host ""
Write-Info "==============================================="
Write-Success "Build successful!"
Write-Info "Output: build\bin\$Configuration\IfcSpacialImport.dll"
Write-Info "==============================================="
exit 0
