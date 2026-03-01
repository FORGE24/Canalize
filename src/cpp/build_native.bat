@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"
echo Compiling native library...

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

:: 1. Try to initialize MSVC environment using vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "!VSWHERE!" (
    for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VS_PATH=%%i"
    )
    if defined VS_PATH (
        if exist "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" (
            echo Found Visual Studio at: !VS_PATH!
            echo Initializing MSVC environment...
            call "!VS_PATH!\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
            if !errorlevel! equ 0 (
                echo MSVC environment initialized.
                goto :compile_cl
            ) else (
                echo Failed to initialize MSVC environment.
            )
        )
    )
)

:: 2. Check if cl.exe is already in PATH
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo Found cl.exe in PATH. Assuming environment is set.
    goto :compile_cl
)

:: 3. Check for MinGW at specific path
if exist "F:\MinGW\bin\g++.exe" (
    set "GPP=F:\MinGW\bin\g++.exe"
    echo Found MinGW at F:\MinGW\bin\g++.exe
    goto :compile_gpp
)

:: 4. Check for g++ in PATH
where g++ >nul 2>nul
if %errorlevel% equ 0 (
    set "GPP=g++"
    echo Found g++ in PATH
    goto :compile_gpp
)

echo Error: No suitable C++ compiler found (cl.exe or g++).
goto :fail

:compile_cl
echo Using MSVC (cl.exe)...
echo JAVA_HOME is: "%JAVA_HOME%"
if not exist "%JAVA_HOME%\include\jni.h" (
    echo ERROR: jni.h not found at "%JAVA_HOME%\include\jni.h"
    goto :fail
)

if exist build rmdir /s /q build
mkdir build
cd build
:: /LD = Create DLL, /Fe: = Output Name, /MD = Multithreaded DLL Runtime, /O2 = Optimize, /arch:AVX2 = Enable AVX2, /EHsc = Enable C++ Exceptions
cl /nologo /LD /MD /O2 /arch:AVX2 /EHsc /Fe:canalize_native.dll /I"%JAVA_HOME%\include" /I"%JAVA_HOME%\include\win32" ..\jni.cpp ..\src\TerrainGen.cpp ..\src\Carver.cpp ..\src\Decorator.cpp ..\src\WorldLoader.cpp
if %errorlevel% neq 0 (
    echo MSVC compilation failed with error level %errorlevel%
    goto :fail
)
echo Build successful with MSVC!
goto :finish

:compile_gpp
echo Using MinGW (%GPP%)...
if exist build rmdir /s /q build
mkdir build
cd build
"%GPP%" -shared -o canalize_native.dll -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" ..\jni.cpp ..\src\TerrainGen.cpp ..\src\Carver.cpp ..\src\Decorator.cpp ..\src\WorldLoader.cpp -static-libgcc -static-libstdc++ -Wl,--add-stdcall-alias -mavx2 -O3
if %errorlevel% neq 0 (
    echo MinGW compilation failed with error level %errorlevel%
    goto :fail
)
echo Build successful with MinGW!
goto :finish

:finish
echo Copying native library to resources...
if not exist ..\..\..\src\main\resources\natives mkdir ..\..\..\src\main\resources\natives
copy /Y canalize_native.dll ..\..\..\src\main\resources\natives\
copy /Y canalize_native.dll ..\..\..\
cd ..
exit /b 0

:fail
echo Compilation FAILED.
if exist ..\..\..\src\main\resources\natives\canalize_native.dll (
    echo Found existing native library, keeping it.
    cd ..
    exit /b 0
)

echo Creating a DUMMY DLL to allow Gradle build to proceed.
echo WARNING: Native features will NOT work in-game.
echo. > canalize_native.dll
if not exist ..\..\..\src\main\resources\natives mkdir ..\..\..\src\main\resources\natives
copy /Y canalize_native.dll ..\..\..\src\main\resources\natives\
copy /Y canalize_native.dll ..\..\..\
cd ..
exit /b 0
