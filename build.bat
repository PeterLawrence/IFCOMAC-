@echo off
REM CMake build helper script for IfcSpacialImport
REM Usage: build.bat [Debug|Release] [clean]

setlocal enabledelayedexpansion

REM Defaults
set CONFIG=Debug
set ACTION=build
set GENERATOR=Visual Studio 15 2017
set PLATFORM=x64

REM Parse arguments
if "%1"=="" goto :default_config
if /i "%1"=="Debug" set CONFIG=Debug
if /i "%1"=="Release" set CONFIG=Release
if /i "%1"=="clean" set ACTION=clean
if /i "%1"=="CleanAll" set ACTION=cleanall

if "%2"=="" goto :run
if /i "%2"=="clean" set ACTION=clean
if /i "%2"=="CleanAll" set ACTION=cleanall

:run
echo.
echo ===============================================
echo IfcSpacialImport CMake Build Helper
echo ===============================================
echo Configuration: %CONFIG%
echo Generator: %GENERATOR%
echo Platform: %PLATFORM%
echo Action: %ACTION%
echo.

REM Handle CleanAll
if /i "%ACTION%"=="cleanall" (
    echo Removing build directory...
    if exist build (
        rmdir /s /q build
        echo Build directory removed.
    )
    echo.
    echo Run this script again to configure and build.
    exit /b 0
)

REM Handle clean
if /i "%ACTION%"=="clean" (
    echo Cleaning build...
    if exist build (
        cd build
        cmake --build . --config %CONFIG% --target clean
        cd ..
    )
    echo Clean complete.
    exit /b 0
)

REM Ensure build directory exists
if not exist build (
    mkdir build
    echo Created build directory.
)

REM Configure
echo.
echo Configuring with CMake...
cd build
cmake .. -G "%GENERATOR%" -A %PLATFORM%
if errorlevel 1 (
    echo Configuration failed!
    cd ..
    exit /b 1
)
cd ..

echo Configuration complete.
echo.

REM Build
echo Building %CONFIG% configuration...
cmake --build build --config %CONFIG%
if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo.
echo ===============================================
echo Build successful!
echo Output: build\bin\%CONFIG%\IfcSpacialImport.dll
echo ===============================================
exit /b 0

:default_config
echo IfcSpacialImport CMake Build Helper
echo.
echo Usage:
echo   build.bat                    - Build Debug configuration
echo   build.bat Release            - Build Release configuration
echo   build.bat clean              - Clean build artifacts
echo   build.bat CleanAll           - Remove build directory completely
echo.
echo Examples:
echo   build.bat                    # Debug build
echo   build.bat Release            # Release build
echo   build.bat Debug clean        # Clean Debug build
echo.
cd build >nul 2>&1
if errorlevel 1 (
    echo Build directory not found. Running initial configuration...
    echo.
    call %0 Debug
) else (
    echo Performing default Debug build...
    cd ..
    call %0 Debug
)
exit /b %ERRORLEVEL%
