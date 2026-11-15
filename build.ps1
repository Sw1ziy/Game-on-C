param(
    [switch]$Run,
    [string]$MsysRoot = 'C:\msys64'
)

$ErrorActionPreference = 'Stop'
$taskBin = Join-Path $MsysRoot 'ucrt64\bin'
$taskCompiler = Join-Path $taskBin 'gcc.exe'
$taskInclude = Join-Path $MsysRoot 'ucrt64\include\ncurses'
$taskSource = Join-Path $PSScriptRoot 'game.c'
$taskOutput = Join-Path $PSScriptRoot 'betflip.exe'

if (-not (Test-Path -LiteralPath $taskCompiler)) {
    throw "MSYS2 UCRT64 GCC not found: $taskCompiler. See README.md for installation."
}
if (-not (Test-Path -LiteralPath (Join-Path $taskInclude 'ncurses.h'))) {
    throw 'ncurses headers not found. Install mingw-w64-ucrt-x86_64-ncurses in MSYS2.'
}

$taskPreviousPath = $env:Path
$taskPreviousTerminfo = $env:TERMINFO
try {
    $env:Path = "$taskBin;$taskPreviousPath"
    $env:TERMINFO = (Join-Path $MsysRoot 'ucrt64\lib\terminfo').Replace('\', '/')
    & $taskCompiler -std=c11 -Wall -Wextra -Wpedantic "-I$taskInclude" $taskSource -lncursesw -o $taskOutput
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE."
    }
    Write-Host "Built: $taskOutput"

    if ($Run) {
        & $taskOutput
        if ($LASTEXITCODE -ne 0) {
            throw "Program exited with code $LASTEXITCODE."
        }
    }
}
finally {
    $env:Path = $taskPreviousPath
    $env:TERMINFO = $taskPreviousTerminfo
}
