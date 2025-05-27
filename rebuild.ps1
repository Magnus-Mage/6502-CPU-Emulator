param (
    [switch]$Build,
    [switch]$Test,
    [switch]$Run,
    [switch]$Watch,
    [switch]$Help
)

$BuildDir = "build"
$TestExec = ".\build\emulator_tests.exe"
$RunExec = ".\build\emulator.exe"
$SrcDir = "src"
$TestDir = "tests"

function Show-Help {
    Write-Host "Usage: .\rebuild.ps1 [-Build] [-Test] [-Run] [-Watch] [-Help]"
    Write-Host "`nOptions:"
    Write-Host "  -Build    Rebuild the project"
    Write-Host "  -Test     Run tests using ctest"
    Write-Host "  -Run      Run the emulator executable"
    Write-Host "  -Watch    Watch for file changes and rebuild"
    Write-Host "  -Help     Show this help message"
}

function Rebuild {
    Write-Host "[INFO] Building project..."
    cmake --build $BuildDir
}

function Run-Tests {
    Write-Host "[INFO] Running tests..."
    ctest --test-dir $BuildDir
}

function Run-Executable {
    Write-Host "[INFO] Running emulator..."
    & $RunExec
}

function Watch-And-Build {
    Write-Host "[INFO] Watching for changes in $SrcDir and $TestDir..."
    while ($true) {
        Get-ChildItem -Recurse $SrcDir, $TestDir -Include *.cpp, *.h |
            Wait-Event -SourceIdentifier FileChanged -Timeout 1 | Out-Null
        Write-Host "`n[INFO] Change detected. Rebuilding..."
        Rebuild
    }
}

if ($Help -or (!($Build -or $Test -or $Run -or $Watch))) {
    Show-Help
    exit
}

if ($Build) {
    Rebuild
}

if ($Test) {
    Run-Tests
}

if ($Run) {
    Run-Executable
}

if ($Watch) {
    Write-Host "[WARN] Watch mode in PowerShell is limited. Consider using a file watcher like 'entr' or VS Code auto-tasks."
}

