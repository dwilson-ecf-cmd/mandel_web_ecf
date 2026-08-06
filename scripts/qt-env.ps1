param(
    [switch]$Persist
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$settings = [ordered]@{
    JAVA_HOME = 'D:\Dev\Java\jdk-21'
    ANDROID_HOME = 'D:\Android\Sdk'
    ANDROID_SDK_ROOT = 'D:\Android\Sdk'
    ANDROID_NDK_ROOT = 'D:\Android\Sdk\ndk\27.2.12479018'
    ANDROID_USER_HOME = 'D:\Android\User'
    ANDROID_EMULATOR_HOME = 'D:\Android\User'
    ANDROID_AVD_HOME = 'D:\Android\Avd'
    GRADLE_USER_HOME = 'D:\Android\Gradle'
    MANDEL_QT_ROOT = 'C:\Qt\6.11.1'
    MANDEL_QT_TOOLS = 'C:\Qt\Tools'
    MANDEL_BUILD_ROOT = 'D:\Build\mandel_web_ecf'
    MANDEL_MSYS_ROOT = 'C:\msys64\mingw64'
}

foreach ($entry in $settings.GetEnumerator()) {
    Set-Item -Path ("Env:" + $entry.Key) -Value $entry.Value
    if ($Persist) {
        [Environment]::SetEnvironmentVariable($entry.Key, $entry.Value, 'User')
    }
}

$toolPaths = @(
    'C:\Qt\6.11.1\mingw_64\bin'
    'C:\Qt\Tools\mingw1310_64\bin'
    'C:\Qt\Tools\CMake_64\bin'
    'C:\Qt\Tools\Ninja'
    'D:\Dev\Java\jdk-21\bin'
    'D:\Android\Sdk\platform-tools'
    'D:\Android\Sdk\cmdline-tools\latest\bin'
    'D:\Android\Sdk\emulator'
)

$processPath = @($env:PATH -split ';' | Where-Object { $_ })
for ($index = $toolPaths.Count - 1; $index -ge 0; --$index) {
    $candidate = $toolPaths[$index]
    if (-not ($processPath | Where-Object { $_.TrimEnd('\') -ieq $candidate.TrimEnd('\') })) {
        $processPath = @($candidate) + $processPath
    }
}
$env:PATH = $processPath -join ';'

if ($Persist) {
    $userPath = @([Environment]::GetEnvironmentVariable('Path', 'User') -split ';' |
        Where-Object { $_ })
    foreach ($candidate in $toolPaths[4..7]) {
        if (-not ($userPath | Where-Object { $_.TrimEnd('\') -ieq $candidate.TrimEnd('\') })) {
            $userPath += $candidate
        }
    }
    [Environment]::SetEnvironmentVariable('Path', ($userPath -join ';'), 'User')
}

[pscustomobject]$settings
