@echo off
setlocal

:: Define usage
if "%1"=="" (
    echo Usage:
    echo   rebuild.bat -build       ^: Build the project
    echo   rebuild.bat -watch       ^: Watch for changes and rebuild
    echo   rebuild.bat -run         ^: Run the emulator
    echo   rebuild.bat -test        ^: Run tests
    exit /b
)

:: Build the project
if "%1"=="-build" (
    cmake -S . -B build -G "MinGW Makefiles"
    cmake --build build
    exit /b
)

:: Watch mode: Check for changes every 2 seconds
if "%1"=="-watch" (
    echo Watching for changes...
    :watch_loop
    timeout /t 2 >nul
    cmake --build build
    goto watch_loop
)

:: Run the emulator
if "%1"=="-run" (
    if exist build\emulator_tests.exe (
        build\emulator_tests.exe
    ) else (
        echo emulator_tests.exe not found. Did you build?
    )
    exit /b
)

:: Run tests
if "%1"=="-test" (
    if exist build\emulator_tests.exe (
        build\emulator_tests.exe
    ) else (
        echo emulator_tests.exe not found. Did you build?
    )
    exit /b
)

echo Invalid option: %1
exit /b

