@echo off
REM ============================================================
REM   小型社交网络系统 —— 一键运行脚本（Windows 双击）
REM
REM   用法：双击本文件即可。
REM   功能：编译，并可选择自动演示 / 交互式体验 / 运行全部测试。
REM
REM   注意：需要 C 编译器。若未安装 gcc，请先安装 MinGW-w64
REM         （https://www.mingw-w64.org/）并勾选加入 PATH。
REM ============================================================
setlocal EnableDelayedExpansion
cd /d "%~dp0"

REM ---- 环境检查 ----
where gcc >nul 2>nul
if errorlevel 1 (
    echo.
    echo [X] 未检测到 gcc 编译器。
    echo.
    echo     请先安装 MinGW-w64 并勾选 "Add to PATH"：
    echo     https://www.mingw-w64.org/
    echo     或安装 TDM-GCC / MSYS2 后重新双击本文件。
    echo.
    pause
    exit /b 1
)

REM ---- 构建选择：mingw32-make / make / 直接 gcc ----
set "MAKEBIN="
where mingw32-make >nul 2>nul && set "MAKEBIN=mingw32-make"
if not defined MAKEBIN (
    where make >nul 2>nul && set "MAKEBIN=make"
)

:build
echo.
echo ------------------------------------------------------------
echo   正在编译（请稍候）...
echo ------------------------------------------------------------
if defined MAKEBIN (
    %MAKEBIN% all >nul 2>&1
    if exist build\social_network.exe goto built
    %MAKEBIN% app >nul 2>&1
    if exist build\social_network.exe goto built
)
if not exist build mkdir build
gcc -std=c11 -Wall -O2 -Iinclude -o build\social_network.exe src\*.c -lm
if not exist build\social_network.exe (
    echo [X] 编译失败，请检查源码与编译器环境。
    pause
    exit /b 1
)

:built
echo [OK] 构建完成：build\social_network.exe
set "HAVE_MAKE="
if defined MAKEBIN set "HAVE_MAKE=1"

:menu
cls
echo ============================================================
echo        小型社交网络系统 —— 一键运行
echo        （数据结构课程设计 · 202532110113 刘亦轩）
echo ============================================================
echo   [1] 自动演示：回放 8 用户示例会话（对照报告第 4 节）
echo   [2] 交互式体验：手动操作菜单（20 用户数据）
echo   [3] 运行全部测试（make test；需要 make）
echo   [0] 退出
echo.
set "choice="
set /p choice=请输入编号 [0-3]：
if "%choice%"=="1" goto demo
if "%choice%"=="2" goto interact
if "%choice%"=="3" goto tests
if "%choice%"=="0" exit /b 0
goto menu

:demo
echo.
echo  ====== 自动演示：8 用户示例会话（需求文档表 1） ======
echo.
build\social_network.exe data\users8.txt < examples\session8.txt
echo.
echo  完整输出转录见 examples\session8.out.txt
pause
goto menu

:interact
echo.
echo  ====== 交互模式（20 用户数据，退出请在菜单选 0） ======
echo.
build\social_network.exe data\users20.txt
pause
goto menu

:tests
echo.
if not defined HAVE_MAKE (
    echo [X] 未检测到 make，无法运行测试。请安装 MinGW-w64 自带的 mingw32-make。
    pause
    goto menu
)
%MAKEBIN% test
pause
goto menu