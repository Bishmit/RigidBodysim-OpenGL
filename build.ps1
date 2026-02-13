# ================= CONFIG =================
$vcpkgPath = "C:/Tools/vcpkg"     # Change if your vcpkg is elsewhere
$generator = "Visual Studio 17 2022"
$arch = "x64"
$buildType = "Release"
$buildDir = "build"
# =========================================

Write-Host "==== OpenGLPhysicsEngine Build Script ====" -ForegroundColor Cyan

# Create build directory if it doesn't exist
if (!(Test-Path $buildDir)) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

Set-Location $buildDir

Write-Host "Configuring project with CMake..."
cmake .. `
  -G "$generator" `
  -A $arch `
  -DCMAKE_TOOLCHAIN_FILE="$vcpkgPath/scripts/buildsystems/vcpkg.cmake"

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Building project..."
cmake --build . --config $buildType

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Build successful!" -ForegroundColor Green
Write-Host "Executable location:"
Write-Host "build/$buildType/main.exe"
