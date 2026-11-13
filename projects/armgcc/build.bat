@echo off
rem bootloader构建脚本: armgcc交叉编译HC32F460
cd /d %~dp0
cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake
cmake --build build
pause
