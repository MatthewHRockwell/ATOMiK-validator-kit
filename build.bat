@echo off
echo [BUILD] Compiling ATOMiK Kernel Simulator...
gcc aos.c atomik_core_sim.c -o aos.exe -lws2_32
if %errorlevel% neq 0 (
    echo [ERR] Compilation Failed!
    pause
    exit /b %errorlevel%
)
echo [SUCCESS] Built aos.exe
echo Run 'aos.exe' to start the kernel.
pause
