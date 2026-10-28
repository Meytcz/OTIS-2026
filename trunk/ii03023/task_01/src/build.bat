@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo ===== [1/4] CMake configure =====
cmake -S . -B build
if errorlevel 1 goto :error

echo ===== [2/4] CMake build =====
cmake --build build --config Release
if errorlevel 1 goto :error

echo ===== [3/4] Run program =====
if exist build\Release\ii03023_lab1.exe (
    build\Release\ii03023_lab1.exe
) else (
    build\ii03023_lab1.exe
)
echo ===== [4/4] Plot (Python + matplotlib) =====
python plot.py

echo ===== DONE =====
pause
exit /b 0

:error
echo BUILD FAILED
pause
exit /b 1
