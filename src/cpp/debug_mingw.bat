@echo off
echo Testing MinGW Environment...
echo PATH=%PATH%
echo.
echo g++ version:
"F:\MinGW\bin\g++.exe" --version
echo.
echo Creating test.cpp...
echo int main() { return 0; } > test.cpp
echo.
echo Compiling test.cpp...
"F:\MinGW\bin\g++.exe" test.cpp -o test.exe -v
if %errorlevel% neq 0 (
    echo Compilation FAILED!
) else (
    echo Compilation SUCCESS!
    del test.exe
)
del test.cpp
pause
