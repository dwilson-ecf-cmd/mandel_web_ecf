param(
    [switch]$SkipBuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

. (Join-Path $PSScriptRoot 'qt-env.ps1') | Out-Null

function Assert-Path {
    param([string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "Required path is missing: $Path"
    }
}

$requiredPaths = @(
    'C:\Qt\6.11.1\mingw_64\lib\cmake\Qt6\Qt6Config.cmake'
    'C:\Qt\6.11.1\android_arm64_v8a\lib\cmake\Qt6\qt.toolchain.cmake'
    'C:\Qt\6.11.1\android_armv7\lib\cmake\Qt6\qt.toolchain.cmake'
    'C:\Qt\6.11.1\android_x86\lib\cmake\Qt6\qt.toolchain.cmake'
    'C:\Qt\6.11.1\android_x86_64\lib\cmake\Qt6\qt.toolchain.cmake'
    'C:\Qt\Tools\QtCreator\bin\qtcreator.exe'
    'C:\Qt\Tools\CMake_64\bin\cmake.exe'
    'C:\Qt\Tools\Ninja\ninja.exe'
    'C:\Qt\Tools\mingw1310_64\bin\g++.exe'
    'D:\Dev\Java\jdk-21\bin\javac.exe'
    'D:\Android\Sdk\platform-tools\adb.exe'
    'D:\Android\Sdk\cmdline-tools\latest\bin\sdkmanager.bat'
    'D:\Android\Sdk\platforms\android-36\android.jar'
    'D:\Android\Sdk\build-tools\36.0.0\aapt2.exe'
    'D:\Android\Sdk\build-tools\36.0.0\apksigner.bat'
    'D:\Android\Sdk\ndk\27.2.12479018\build\cmake\android.toolchain.cmake'
    'D:\Android\Sdk\ndk\27.2.12479018\toolchains\llvm\prebuilt\windows-x86_64\bin\clang++.exe'
    'D:\Android\Sdk\emulator\emulator.exe'
    'D:\Android\Sdk\system-images\android-36\google_apis\x86_64\package.xml'
    'D:\Android\Sdk\extras\google\usb_driver\android_winusb.inf'
    'D:\Android\Avd\mandel_api36_x86_64.ini'
)
foreach ($path in $requiredPaths) {
    Assert-Path $path
}

foreach ($entry in $settings.GetEnumerator()) {
    $persisted = [Environment]::GetEnvironmentVariable($entry.Key, 'User')
    if ($persisted -ne $entry.Value) {
        throw "Persistent user environment mismatch for $($entry.Key): $persisted"
    }
}

$persistentToolPaths = @(
    'D:\Dev\Java\jdk-21\bin'
    'D:\Android\Sdk\platform-tools'
    'D:\Android\Sdk\cmdline-tools\latest\bin'
    'D:\Android\Sdk\emulator'
)
$userPath = @([Environment]::GetEnvironmentVariable('Path', 'User') -split ';' |
    Where-Object { $_ })
foreach ($candidate in $persistentToolPaths) {
    if (-not ($userPath | Where-Object { $_.TrimEnd('\') -ieq $candidate.TrimEnd('\') })) {
        throw "Persistent user PATH is missing: $candidate"
    }
}

$qmakeVersion = & 'C:\Qt\6.11.1\mingw_64\bin\qmake.exe' -query QT_VERSION
if ($qmakeVersion.Trim() -ne '6.11.1') {
    throw "Unexpected Qt version: $qmakeVersion"
}

$javacVersion = & 'D:\Dev\Java\jdk-21\bin\javac.exe' -version 2>&1
if ($javacVersion -notmatch '^javac 21\.') {
    throw "Unexpected JDK version: $javacVersion"
}

$ndkProperties = Get-Content -LiteralPath 'D:\Android\Sdk\ndk\27.2.12479018\source.properties' -Raw
if ($ndkProperties -notmatch 'Pkg.Revision\s*=\s*27\.2\.12479018') {
    throw 'The installed NDK revision does not match Qt 6.11.1.'
}

$qtCreatorProfiles = Join-Path $env:APPDATA 'QtProject\qtcreator\profiles.xml'
Assert-Path $qtCreatorProfiles
$profileText = Get-Content -LiteralPath $qtCreatorProfiles -Raw
if ($profileText -notmatch [regex]::Escape('Desktop Qt %{Qt:Version} MinGW 64-bit')) {
    throw 'Qt Creator desktop kit is missing.'
}
foreach ($abi in 'arm64-v8a', 'armeabi-v7a', 'x86', 'x86_64') {
    if ($profileText -notmatch [regex]::Escape("for Android $abi")) {
        throw "Qt Creator kit is missing for $abi"
    }
}

$qtCreatorIni = Join-Path $env:APPDATA 'QtProject\QtCreator.ini'
Assert-Path $qtCreatorIni
$qtCreatorConfiguration = Get-Content -LiteralPath $qtCreatorIni -Raw
foreach ($expectedSetting in @(
    'DefaultNdkLocation=D:/Android/Sdk/ndk/27.2.12479018'
    'OpenJDKLocation=D:/Dev/Java/jdk-21'
    'SDKLocation=D:/Android/Sdk'
)) {
    if ($qtCreatorConfiguration -notmatch [regex]::Escape($expectedSetting)) {
        throw "Qt Creator Android setting is missing: $expectedSetting"
    }
}

Write-Output 'Qt 6.11.1, JDK 21, Android API 36, NDK r27c, Qt Creator, and all ABI kits are valid.'

if (-not $SkipBuild) {
    $repositoryRoot = Split-Path -Parent $PSScriptRoot
    Push-Location $repositoryRoot
    try {
        & 'C:\Qt\Tools\CMake_64\bin\cmake.exe' --preset qt-desktop-debug
        if ($LASTEXITCODE -ne 0) { throw 'Desktop Qt configure failed.' }
        & 'C:\Qt\Tools\CMake_64\bin\cmake.exe' --build --preset qt-desktop-debug --parallel
        if ($LASTEXITCODE -ne 0) { throw 'Desktop Qt build failed.' }
        & 'C:\Qt\Tools\CMake_64\bin\ctest.exe' --preset qt-desktop-debug
        if ($LASTEXITCODE -ne 0) { throw 'Desktop Qt tests failed.' }
    }
    finally {
        Pop-Location
    }
}
