# STM32G474 BMS Project

Utworzyłem nową branczkę, usunąłem z niej śmiecio-rozpraszacze. Całość opiera się na CMake, co pozwala na korzystanie z 3 różnych edytorów/IDE - CLion, CubeIDE, VSCode (jak na razie VSCode średnio działa).

## Quick Setup

### CubeIDE Import
1. Import → Existing projects into workspace
2. Wybierz repo jako folder
3. Zaznacz projekty: `app` i `custom-G474` 
4. Pliki projektowe CubeIDE są automatycznie generowane przez CMake więc IDE powinien znaleźć

### BMS Task Implementation
Główna pętla zadania BMS z implementacją wysyłania po CAN:
- **Lokalizacja**: [app/Src/Tasks/battery_monitor_task.cpp](https://github.com/Girmoire/STM32G474/blob/feature/vescan-comunication-on-custom-G474/app/Src/Tasks/battery_monitor_task.cpp)
- **Funkcjonalność**: 
  - Monitoring baterii BQ40Z80
  - Telemetria w strukturze packed
  - Transmisja FDCAN używając VESC protocol
  - Platform-aware logging (UART dla G474)

## Build Requirements

### STM32CubeCLT (Recommended)
Do buildu wymagane jest zainstalowanie **STM32CubeCLT** - znacznie prostsze ścieżki:
```
C:\ST\STM32CubeCLT_1.18.0\
├── CMake\bin\cmake.exe
├── GNU-tools-for-STM32\bin\arm-none-eabi-gcc.exe
└── Ninja\bin\ninja.exe
```

### CubeIDE Toolchain (Alternative) 
Można podać ścieżki do toolchainu zbundlowanego z CubeIDE ale te ścieżki są o wiele dłuższe i mniej przyjemne.

## Build Commands

Dam też komendy do ręcznego buildu z konsoli - są zbędne bo jest pełna integracja z IDE ale jakby coś się nie kompilowało to łatwiej się czyta w terminalu.

### Make (Simple)
Jeśli masz make:
```bash
make clean
make all
```

### CMake (Advanced)
Jeśli masz cmake - komenda jest powalona:

**Generacja build systemu:**
```bash
C:\ST\STM32CubeCLT_1.18.0\CMake\bin\cmake.exe -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=C:/ST/STM32CubeCLT_1.18.0/Ninja/bin/ninja.exe -DCMAKE_C_COMPILER=C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe -DCMAKE_CXX_COMPILER=C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-c++.exe -G Ninja -S . -B cmake-build-debug
```

**Kompilacja:**
```bash
C:\ST\STM32CubeCLT_1.18.0\CMake\bin\cmake.exe --build cmake-build-debug --target STM32G474 -j 10
```

### Build from custom-G474 directory
```bash
cd boards/custom-G474
# Configuration
C:\ST\STM32CubeCLT_1.18.0\CMake\bin\cmake.exe -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -G Ninja
# Build  
C:\ST\STM32CubeCLT_1.18.0\CMake\bin\cmake.exe --build cmake-build-debug --target STM32G474 -j 10
```

## Project Structure

```
STM32G474/
├── app/                          # Shared application library
│   └── Src/Tasks/
│       ├── battery_monitor_task.cpp    # Main BMS task with FDCAN
│       ├── vescan_task.cpp             # VESC CAN communication  
│       └── hal_implementations.cpp     # Platform HAL overrides
├── boards/custom-G474/           # STM32G474 board-specific code
│   ├── Core/                     # STM32CubeMX generated
│   ├── cmake-build-debug/        # Build output
│   └── STM32G474.ioc            # Hardware configuration
└── CLAUDE.md                    # Development guidelines
```

## Hardware
- **MCU**: STM32G474RET6 (Cortex-M4F, 170MHz)
- **BMS IC**: BQ40Z80 (I2C communication)
- **CAN**: FDCAN for telemetry transmission
- **Debug**: UART2 console output

## IDE Support Status
- ✅ **CLion**: Full CMake integration, debugging
- ✅ **CubeIDE**: Import projects, auto-generated files  
- ⚠️ **VSCode**: Basic support, debugging needs work

---
For detailed development info see [CLAUDE.md](CLAUDE.md)