@echo off
REM SunnyLand游戏打包脚本
echo ========================================
echo SunnyLand 游戏打包工具
echo ========================================
echo.

REM 第一步：清理并重新配置（避免Debug/Release库冲突）
echo [1/6] 清理旧的构建缓存...
if exist build (
    echo 正在清理build目录...
    rmdir /s /q build
)
echo.

REM 第二步：编译Release版本
echo [2/6] 配置Release版本...
cmake -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

echo.
echo [3/6] 编译Release版本...
cmake --build build --config Release
if errorlevel 1 (
    echo 错误: 编译失败
    pause
    exit /b 1
)
echo Release版本编译完成！
echo.

REM 第三步：创建发布目录
echo [4/6] 创建发布目录...
set PACKAGE_DIR=SunnyLand_Release
if exist %PACKAGE_DIR% rmdir /s /q %PACKAGE_DIR%
mkdir %PACKAGE_DIR%
echo.

REM 第四步：复制可执行文件
echo [5/6] 复制可执行文件...
copy SunnyLand-Windows.exe %PACKAGE_DIR%\ >nul
if errorlevel 1 (
    echo 错误: 未找到可执行文件
    pause
    exit /b 1
)
echo.

REM 第五步：复制SDL3依赖库
echo [6/6] 复制SDL3依赖库...
set SDL3_LIB_PATH=C:\library\SDL3\lib\x64
if exist "%SDL3_LIB_PATH%\SDL3.dll" (
    copy "%SDL3_LIB_PATH%\SDL3.dll" %PACKAGE_DIR%\ >nul
    copy "%SDL3_LIB_PATH%\SDL3_image.dll" %PACKAGE_DIR%\ >nul
    copy "%SDL3_LIB_PATH%\SDL3_mixer.dll" %PACKAGE_DIR%\ >nul
    copy "%SDL3_LIB_PATH%\SDL3_ttf.dll" %PACKAGE_DIR%\ >nul
    echo SDL3库复制完成
) else (
    echo 警告: 未找到SDL3库，请手动复制DLL文件
    echo 路径: %SDL3_LIB_PATH%
)
echo.

REM 第六步：复制资源文件
echo [7/7] 复制资源文件...
xcopy /E /I /Y assets %PACKAGE_DIR%\assets >nul
if errorlevel 1 (
    echo 错误: 资源文件复制失败
    pause
    exit /b 1
)
echo.

REM 创建README
echo 创建README文件...
(
echo ========================================
echo      SunnyLand - 游戏说明
echo ========================================
echo.
echo 游戏名称: SunnyLand
echo 版本: 0.1.0
echo 平台: Windows x64
echo.
echo 运行要求:
echo - Windows 10/11 (64位^)
echo - 显卡支持DirectX 11或更高版本
echo.
echo 运行方法:
echo 双击 SunnyLand-Windows.exe 开始游戏
echo.
echo 注意事项:
echo - 请勿删除assets文件夹及其内容
echo - 请勿删除SDL3相关的DLL文件
echo - 游戏存档保存在assets/save.json
echo.
echo 祝你游戏愉快！
echo ========================================
) > %PACKAGE_DIR%\README.txt

echo.
echo ========================================
echo 打包完成！
echo ========================================
echo.
echo 发布文件位于: %PACKAGE_DIR%\
echo.
echo 文件列表:
dir /B %PACKAGE_DIR%
echo.
echo 下一步操作:
echo 1. 测试游戏是否能正常运行
echo 2. 使用7-Zip或WinRAR压缩整个文件夹
echo 3. 分享压缩包给其他用户
echo.
pause
