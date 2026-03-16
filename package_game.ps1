# SunnyLand 游戏自动打包脚本 (PowerShell版本)
# 使用方法: 右键点击 -> 使用PowerShell运行

param(
    [switch]$SkipBuild = $false,
    [string]$OutputDir = "SunnyLand_Release"
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   SunnyLand 游戏打包工具 (PowerShell)" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否在正确的目录
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Host "错误: 请在项目根目录运行此脚本" -ForegroundColor Red
    Read-Host "按Enter键退出"
    exit 1
}

try {
    # 第一步：清理构建缓存（避免Debug/Release库冲突）
    if (-not $SkipBuild) {
        Write-Host "[1/7] 清理旧的构建缓存..." -ForegroundColor Yellow
        if (Test-Path "build") {
            Remove-Item "build" -Recurse -Force
            Write-Host "✓ 清理build目录" -ForegroundColor Green
        }
        Write-Host ""
    }

    # 第二步：编译Release版本
    if (-not $SkipBuild) {
        Write-Host "[2/7] 配置Release版本..." -ForegroundColor Yellow

        # 配置CMake
        $configResult = cmake -B build -DCMAKE_BUILD_TYPE=Release 2>&1
        if ($LASTEXITCODE -ne 0) {
            Write-Host "错误: CMake配置失败" -ForegroundColor Red
            Write-Host $configResult
            Read-Host "按Enter键退出"
            exit 1
        }

        Write-Host ""

        # 编译
        Write-Host "[3/7] 编译Release版本..." -ForegroundColor Yellow
        $buildResult = cmake --build build --config Release 2>&1
        if ($LASTEXITCODE -ne 0) {
            Write-Host "错误: 编译失败" -ForegroundColor Red
            Write-Host $buildResult
            Read-Host "按Enter键退出"
            exit 1
        }

        Write-Host "✓ Release版本编译完成" -ForegroundColor Green
        Write-Host ""
    } else {
        Write-Host "[1/7] 跳过编译步骤..." -ForegroundColor Yellow
        Write-Host ""
    }

    # 第四步：创建发布目录
    Write-Host "[4/7] 创建发布目录..." -ForegroundColor Yellow
    if (Test-Path $OutputDir) {
        Remove-Item $OutputDir -Recurse -Force
        Write-Host "✓ 清理旧的发布目录" -ForegroundColor Green
    }
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
    Write-Host "✓ 创建新的发布目录: $OutputDir" -ForegroundColor Green
    Write-Host ""

    # 第五步：复制可执行文件
    Write-Host "[5/7] 复制可执行文件..." -ForegroundColor Yellow
    $exePath = "SunnyLand-Windows.exe"
    if (-not (Test-Path $exePath)) {
        Write-Host "错误: 未找到可执行文件 $exePath" -ForegroundColor Red
        Write-Host "请先编译项目" -ForegroundColor Red
        Read-Host "按Enter键退出"
        exit 1
    }

    Copy-Item $exePath "$OutputDir\" -Force

    # 获取文件大小
    $exeSize = (Get-Item $exePath).Length / 1MB
    Write-Host "✓ 复制可执行文件 (大小: $([math]::Round($exeSize, 2)) MB)" -ForegroundColor Green

    # 检查是否为Debug版本（通过文件大小和PDB文件判断）
    if ((Test-Path "SunnyLand-Windows.pdb") -or $exeSize -gt 10) {
        Write-Host "⚠ 警告: 检测到可能是Debug版本！" -ForegroundColor Yellow
        Write-Host "  Debug版本不适合发布给用户，建议使用Release版本" -ForegroundColor Yellow
    }
    Write-Host ""

    # 第六步：复制SDL3依赖库
    Write-Host "[6/7] 复制SDL3依赖库..." -ForegroundColor Yellow
    $sdlLibPath = "C:\library\SDL3\lib\x64"
    $sdlDlls = @("SDL3.dll", "SDL3_image.dll", "SDL3_mixer.dll", "SDL3_ttf.dll")

    $copiedCount = 0
    foreach ($dll in $sdlDlls) {
        $sourcePath = Join-Path $sdlLibPath $dll
        if (Test-Path $sourcePath) {
            Copy-Item $sourcePath "$OutputDir\" -Force
            $copiedCount++
            Write-Host "  ✓ $dll" -ForegroundColor Green
        } else {
            Write-Host "  ⚠ 未找到: $dll" -ForegroundColor Yellow
        }
    }

    if ($copiedCount -eq 0) {
        Write-Host "警告: 未找到SDL3库文件，请手动复制以下DLL到 $OutputDir\" -ForegroundColor Yellow
        $sdlDlls | ForEach-Object { Write-Host "  - $_" -ForegroundColor Yellow }
    } else {
        Write-Host "✓ 成功复制 $copiedCount 个SDL3库文件" -ForegroundColor Green
    }
    Write-Host ""

    # 第七步：复制资源文件
    Write-Host "[7/7] 复制资源文件..." -ForegroundColor Yellow
    if (Test-Path "assets") {
        Copy-Item "assets" "$OutputDir\assets" -Recurse -Force

        # 计算资源文件大小
        $assetsSize = (Get-ChildItem "$OutputDir\assets" -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB
        Write-Host "✓ 复制assets目录 (大小: $([math]::Round($assetsSize, 2)) MB)" -ForegroundColor Green

        # 可选：删除save.json（用户自己的存档）
        $saveFile = "$OutputDir\assets\save.json"
        if (Test-Path $saveFile) {
            Write-Host "  ⚠ 发现存档文件，是否删除？(用户应该有自己的新存档)" -ForegroundColor Yellow
            # 这里可以选择删除或保留
            # Remove-Item $saveFile -Force
        }
    } else {
        Write-Host "警告: 未找到assets目录" -ForegroundColor Red
    }
    Write-Host ""

    # 第八步：创建README
    Write-Host "[8/8] 创建README文件..." -ForegroundColor Yellow

    $readmeContent = @"
========================================
     SunnyLand - 游戏说明
========================================

游戏名称: SunnyLand
版本: 0.1.0
平台: Windows x64
编译日期: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")

运行要求:
---------
- Windows 10/11 (64位)
- 显卡支持DirectX 11或更高版本
- Visual C++ Redistributable 2015-2022 (通常系统已安装)

运行方法:
---------
双击 SunnyLand-Windows.exe 开始游戏

文件说明:
---------
SunnyLand-Windows.exe  - 游戏主程序
SDL3.dll               - SDL3核心库
SDL3_image.dll         - 图像加载库
SDL3_mixer.dll         - 音频混音库
SDL3_ttf.dll           - 字体渲染库
assets/                - 游戏资源文件夹

注意事项:
---------
- 请勿删除assets文件夹及其内容
- 请勿删除SDL3相关的DLL文件
- 游戏存档保存在assets/save.json
- 如遇到DLL缺失错误，请安装 Visual C++ Redistributable:
  https://aka.ms/vs/17/release/vc_redist.x64.exe

故障排除:
---------
1. 游戏无法启动：
   - 确认所有DLL文件都在同一目录
   - 安装最新的Visual C++ Redistributable
   - 检查显卡驱动是否更新

2. 无法保存游戏：
   - 确认对assets文件夹有写入权限
   - 不要将游戏放在Program Files等受保护的目录

3. 音频或图像无法加载：
   - 确认assets文件夹完整
   - 重新下载游戏包

========================================
祝你游戏愉快！
========================================
"@

    $readmeContent | Out-File "$OutputDir\README.txt" -Encoding UTF8
    Write-Host "✓ 创建README.txt" -ForegroundColor Green
    Write-Host ""

    # 总结
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "          打包完成！" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""

    # 显示发布目录内容
    Write-Host "发布文件位于: $OutputDir\" -ForegroundColor Green
    Write-Host ""
    Write-Host "文件列表:" -ForegroundColor Yellow
    Get-ChildItem $OutputDir | ForEach-Object {
        if ($_.PSIsContainer) {
            Write-Host "  📁 $($_.Name)" -ForegroundColor Cyan
        } else {
            $size = $_.Length / 1KB
            Write-Host "  📄 $($_.Name) ($([math]::Round($size, 2)) KB)" -ForegroundColor White
        }
    }
    Write-Host ""

    # 计算总大小
    $totalSize = (Get-ChildItem $OutputDir -Recurse | Measure-Object -Property Length -Sum).Sum / 1MB
    Write-Host "总大小: $([math]::Round($totalSize, 2)) MB" -ForegroundColor Green
    Write-Host ""

    # 下一步操作
    Write-Host "下一步操作:" -ForegroundColor Yellow
    Write-Host "  1. 在本机测试游戏是否能正常运行" -ForegroundColor White
    Write-Host "  2. (推荐) 在另一台电脑上测试" -ForegroundColor White
    Write-Host "  3. 创建压缩包以便分发" -ForegroundColor White
    Write-Host ""

    # 询问是否创建ZIP
    $createZip = Read-Host "是否创建ZIP压缩包？(Y/N)"
    if ($createZip -eq "Y" -or $createZip -eq "y") {
        $zipName = "SunnyLand_v0.1.0_Windows_$(Get-Date -Format 'yyyyMMdd').zip"
        Write-Host ""
        Write-Host "正在创建压缩包: $zipName ..." -ForegroundColor Yellow

        Compress-Archive -Path "$OutputDir\*" -DestinationPath $zipName -Force

        $zipSize = (Get-Item $zipName).Length / 1MB
        Write-Host "✓ 压缩包创建完成 (大小: $([math]::Round($zipSize, 2)) MB)" -ForegroundColor Green
        Write-Host ""
        Write-Host "现在你可以分享这个压缩包给其他用户了！" -ForegroundColor Green
    }

    Write-Host ""
    Write-Host "打包完成！按Enter键退出..." -ForegroundColor Cyan
    Read-Host

} catch {
    Write-Host ""
    Write-Host "错误: $_" -ForegroundColor Red
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    Write-Host ""
    Read-Host "按Enter键退出"
    exit 1
}
