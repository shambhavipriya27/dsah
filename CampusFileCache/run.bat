@echo off
cd /d "%~dp0"
if not defined CC if exist "C:\Users\Shambhavi\Downloads\winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1\mingw64\bin\gcc.exe" set "CC=C:\Users\Shambhavi\Downloads\winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64msvcrt-13.0.0-r1\mingw64\bin\gcc.exe"
python bridge\server.py
pause
