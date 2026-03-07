@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"
echo Compiling native library using CMake...

:: Detect JAVA_HOME if not set
if "%JAVA_HOME%"=="" (
    echo JAVA_HOME not set. Attempting to detect...
    for /f "tokens=2 delims==" %%I in ('java -XshowSettings:properties -version 2^>^&1 ^| findstr "java.home"') do (
        set "JAVA_HOME=%%I"
    )
    :: Remove leading spaces
    for /f "tokens=* delims= " %%I in ("!JAVA_HOME!") do set "JAVA_HOME=%%I"
)
if "%JAVA_HOME%"=="" (
    echo Error: JAVA_HOME not set and could not be detected.
    goto :fail
)
echo Detected JAVA_HOME: "!JAVA_HOME!"

:: Sanitize JAVA_HOME for CMake (replace backslashes with forward slashes to avoid escape issues)
set "JAVA_HOME_CMAKE=!JAVA_HOME:\=/!"

:: Clean previous build
if exist build rmdir /s /q build
mkdir build
cd build

:: Configure with CMake
:: We pass JAVA_HOME to help FindJNI
cmake .. -DJAVA_HOME="!JAVA_HOME_CMAKE!"
if %errorlevel% neq 0 (
    echo CMake configuration failed.
    goto :fail
)

:: Build
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo CMake build failed.
    goto :fail
)

:: Copy output
echo Build successful!
goto :finish

:finish
echo Copying native library to resources...
if exist ..\..\..\build\libs\Release\canalize_native.dll (
    set "DLL_PATH=..\..\..\build\libs\Release\canalize_native.dll"
) else if exist ..\..\..\build\libs\canalize_native.dll (
    set "DLL_PATH=..\..\..\build\libs\canalize_native.dll"
) else if exist Release\canalize_native.dll (
    set "DLL_PATH=Release\canalize_native.dll"
) else if exist canalize_native.dll (
    set "DLL_PATH=canalize_native.dll"
) else (
    echo Error: canalize_native.dll not found.
    goto :fail
)

if not exist ..\..\..\src\main\resources\natives mkdir ..\..\..\src\main\resources\natives
copy /Y "!DLL_PATH!" ..\..\..\src\main\resources\natives\
copy /Y "!DLL_PATH!" ..\..\..\
cd ..
exit /b 0

:fail
echo Compilation FAILED.
cd ..
exit /b 1
