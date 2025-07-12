# PowerShell script to create symlinks for STM32 project structure
# Usage: .\create_symlinks.ps1 [board_name]
# Example: .\create_symlinks.ps1 custom-G474

param(
    [Parameter(Position=0)]
    [string]$Board = "custom-G474"
)

# Function to check if running as administrator
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# Function to restart script with elevated privileges
function Start-ElevatedProcess {
    param(
        [string]$ScriptPath,
        [string]$Arguments
    )
    
    try {
        Write-Host "Requesting administrator privileges..." -ForegroundColor Yellow
        
        $startInfo = New-Object System.Diagnostics.ProcessStartInfo
        $startInfo.FileName = "powershell.exe"
        $startInfo.Arguments = "-ExecutionPolicy Bypass -File `"$ScriptPath`" $Arguments"
        $startInfo.Verb = "RunAs"
        $startInfo.UseShellExecute = $true
        
        $process = [System.Diagnostics.Process]::Start($startInfo)
        
        if ($process) {
            Write-Host "Script restarted with administrator privileges." -ForegroundColor Green
            exit 0
        } else {
            Write-Host "Failed to restart with administrator privileges." -ForegroundColor Red
            exit 1
        }
    }
    catch {
        Write-Host "User declined administrator privileges or elevation failed." -ForegroundColor Red
        Write-Host "Administrator privileges are required to create symbolic links." -ForegroundColor Red
        exit 1
    }
}

# Check if running as administrator, if not, restart with elevation
if (-not (Test-Administrator)) {
    Write-Host "Administrator privileges required for creating symbolic links." -ForegroundColor Yellow
    $scriptPath = $MyInvocation.MyCommand.Path
    $arguments = $Board
    Start-ElevatedProcess -ScriptPath $scriptPath -Arguments $arguments
}

Write-Host "Running with administrator privileges." -ForegroundColor Green

# Validate board parameter
$validBoards = @("custom-G474", "nucleo-H755ZI-Q")
if ($Board -notin $validBoards) {
    Write-Host "Error: Invalid board '$Board'. Valid boards are: $($validBoards -join ', ')" -ForegroundColor Red
    Write-Host "Usage: .\create_symlinks.ps1 [board_name]"
    Write-Host "Example: .\create_symlinks.ps1 custom-G474"
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "Creating symlinks for board: $Board" -ForegroundColor Cyan

# Define board-specific configurations
$boardConfigs = @{
    "custom-G474" = @{
        symlinks = @(
            @{ Path = "Core"; Target = "boards\custom-G474\Core" }
            @{ Path = "Drivers"; Target = "boards\custom-G474\Drivers" }
            @{ Path = "Middlewares"; Target = "boards\custom-G474\Middlewares" }
        )
        fileSymlinks = @(
            @{ Path = "startup_stm32g474xx.s"; Target = "boards\custom-G474\startup_stm32g474xx.s" }
            @{ Path = "STM32G474XX_FLASH.ld"; Target = "boards\custom-G474\STM32G474XX_FLASH.ld" }
        )
    }
    "nucleo-H755ZI-Q" = @{
        symlinks = @(
            @{ Path = "CM4"; Target = "boards\nucleo-H755ZI-Q\CM4" }
            @{ Path = "CM7"; Target = "boards\nucleo-H755ZI-Q\CM7" }
            @{ Path = "Common"; Target = "boards\nucleo-H755ZI-Q\Common" }
            @{ Path = "Drivers"; Target = "boards\nucleo-H755ZI-Q\Drivers" }
            @{ Path = "Middlewares"; Target = "boards\nucleo-H755ZI-Q\Middlewares" }
        )
        fileSymlinks = @(
            @{ Path = "nucleo-H755ZI-Q.ioc"; Target = "boards\nucleo-H755ZI-Q\nucleo-H755ZI-Q.ioc" }
        )
    }
}

# Get configuration for selected board
$config = $boardConfigs[$Board]

# Function to remove existing symlinks/directories
function Remove-ExistingItem {
    param($ItemPath)
    
    if (Test-Path $ItemPath) {
        $item = Get-Item $ItemPath
        if ($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) {
            Write-Host "Removing existing symlink: $ItemPath" -ForegroundColor Yellow
        } else {
            Write-Host "Removing existing item: $ItemPath" -ForegroundColor Yellow
        }
        Remove-Item $ItemPath -Recurse -Force
    }
}

# Create directory symlinks
Write-Host ""
Write-Host "Creating directory symlinks..." -ForegroundColor Green
$successCount = 0
$totalCount = 0

foreach ($link in $config.symlinks) {
    $totalCount++
    Remove-ExistingItem $link.Path
    
    Write-Host "Creating symlink: $($link.Path) -> $($link.Target)"
    try {
        # Verify target exists
        if (-not (Test-Path $link.Target)) {
            Write-Host "  Failed: Target does not exist: $($link.Target)" -ForegroundColor Red
            continue
        }
        
        New-Item -ItemType SymbolicLink -Path $link.Path -Target $link.Target -Force | Out-Null
        Write-Host "  Success: $($link.Path)" -ForegroundColor Green
        $successCount++
    }
    catch {
        Write-Host "  Failed: $($link.Path) - $($_.Exception.Message)" -ForegroundColor Red
    }
}

# Create file symlinks
if ($config.fileSymlinks.Count -gt 0) {
    Write-Host ""
    Write-Host "Creating file symlinks..." -ForegroundColor Green
    foreach ($link in $config.fileSymlinks) {
        $totalCount++
        Remove-ExistingItem $link.Path
        
        Write-Host "Creating file symlink: $($link.Path) -> $($link.Target)"
        try {
            # Verify target exists
            if (-not (Test-Path $link.Target)) {
                Write-Host "  Failed: Target does not exist: $($link.Target)" -ForegroundColor Red
                continue
            }
            
            New-Item -ItemType SymbolicLink -Path $link.Path -Target $link.Target -Force | Out-Null
            Write-Host "  Success: $($link.Path)" -ForegroundColor Green
            $successCount++
        }
        catch {
            Write-Host "  Failed: $($link.Path) - $($_.Exception.Message)" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "Symlink creation completed for board: $Board!" -ForegroundColor Cyan
Write-Host "Results: $successCount of $totalCount symlinks created successfully." -ForegroundColor White

if ($successCount -eq $totalCount) {
    Write-Host "All symlinks created successfully!" -ForegroundColor Green
} elseif ($successCount -gt 0) {
    Write-Host "Some symlinks failed to create. Check the output above for details." -ForegroundColor Yellow
} else {
    Write-Host "No symlinks were created successfully. Check target paths and permissions." -ForegroundColor Red
}

Write-Host ""
Write-Host "You can now build the project with:" -ForegroundColor Cyan
Write-Host "  cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=$Board" -ForegroundColor Yellow
Write-Host "  cmake --build cmake-build-debug" -ForegroundColor Yellow

# Note about privilege management
Write-Host ""
Write-Host "Note: PowerShell will automatically return to normal user privileges" -ForegroundColor Gray
Write-Host "when this elevated process terminates." -ForegroundColor Gray

Write-Host ""
Write-Host "Press Enter to exit..."
Read-Host