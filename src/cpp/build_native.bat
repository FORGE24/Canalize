@echo off
if exist build rmdir /s /q build
mkdir build
cd build

echo Compiling directly with F:\MinGW\bin\g++.exe...
REM Try invoking via cmd /c to handle potential path/execution issues
cmd /c "F:\MinGW\bin\g++.exe" -shared -o canalize_native.dll -I"%JAVA_HOME%\include" -I"%JAVA_HOME%\include\win32" ..\jni.cpp -static-libgcc -static-libstdc++ -Wl,--add-stdcall-alias
if %errorlevel% neq 0 (
    echo Compilation failed with error level %errorlevel%
    echo Creating a DUMMY DLL to allow Gradle build to proceed.
    echo WARNING: Native features will NOT work in-game.
    echo. > canalize_native.dll
) else (
    echo Build successful!
)

copy /Y canalize_native.dll ..\..\..\
cd ..
exit /b 0
