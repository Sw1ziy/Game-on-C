param(
    [switch]$Run,
    [string]$MsysRoot = 'C:\msys64'
)

$ErrorActionPreference = 'Stop'
$taskBin = Join-Path $MsysRoot 'ucrt64\bin'
$taskTools = Join-Path $MsysRoot 'usr\bin'
$taskMake = Join-Path $taskTools 'make.exe'

if (-not (Test-Path -LiteralPath $taskMake)) {
    throw 'MSYS2 make not found. Install make in MSYS2; see README.md.'
}

$taskPreviousPath = $env:Path
$taskPreviousTerminfo = $env:TERMINFO
try {
    $env:Path = "$taskBin;$taskTools;$taskPreviousPath"
    $env:TERMINFO = (Join-Path $MsysRoot 'ucrt64\lib\terminfo').Replace('\', '/')
    $taskTarget = if ($Run) { 'run' } else { 'all' }
    $taskMakeRoot = $MsysRoot.Replace('\', '/')
    & $taskMake -C $PSScriptRoot "MSYS2_ROOT=$taskMakeRoot" $taskTarget
    if ($LASTEXITCODE -ne 0) {
        throw "make $taskTarget failed with exit code $LASTEXITCODE."
    }
}
finally {
    $env:Path = $taskPreviousPath
    $env:TERMINFO = $taskPreviousTerminfo
}
