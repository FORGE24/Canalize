@echo off
setlocal enabledelayedexpansion
echo Compiling native library...

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

:: 2. Check if cl.exe is already in PATH (e.g. running from Developer Command Prompt)
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
if exist build rmdir /s /q build
mkdir build
cd build
:: /LD = Create DLL, /Fe: = Output Name, /MD = Multithreaded DLL Runtime
cl /nologo /LD /MD /Fe:canalize_native.dll /I"%JAVA_HOME%\include" /I"%JAVA_HOME%\include\win32" ..\jni.cpp
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
"%GPP%" -shared -o canalize_native.dll -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" ..\jni.cpp -static-libgcc -static-libstdc++ -Wl,--add-stdcall-alias
if %errorlevel% neq 0 (
    echo MinGW compilation failed with error level %errorlevel%
    goto :fail
)
echo Build successful with MinGW!
goto :finish

:finish
copy /Y canalize_native.dll ..\..\..\
cd ..
exit /b 0

:fail
echo Creating a DUMMY DLL to allow Gradle build to proceed.
echo WARNING: Native features will NOT work in-game.
echo. > canalize_native.dll
copy /Y canalize_native.dll ..\..\..\
cd ..
exit /b 0
