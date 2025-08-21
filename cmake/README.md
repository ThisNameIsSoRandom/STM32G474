# STM32CubeIDE Integration

This CMake module automatically generates STM32CubeIDE project files for easy importing and debugging.

## Overview

The `stm32cubeide.cmake` module creates Eclipse project files (`.project` and `.settings/`) that allow STM32CubeIDE to recognize and import CMake-based STM32 projects.

## Generated Files

For each board, the following files are automatically generated during CMake configuration:

- `.project` - Eclipse project descriptor with STM32CubeIDE natures
- `.cproject` - Eclipse CDT build configuration with managed build settings
- `.settings/language.settings.xml` - Language and compiler settings
- `.settings/org.eclipse.cdt.core.prefs` - CDT preferences

The `.cproject` file contains complete STM32CubeIDE external build configuration including:
- Debug and Release build configurations with CMake/Ninja integration
- External builder using STM32CubeCLT CMake (replaces managed build)
- MCU-specific compiler flags (-mcpu, -mfpu, -mfloat-abi)
- HAL defines (USE_HAL_DRIVER, STM32xxx)
- Include paths for HAL, CMSIS, FreeRTOS, and application code
- Linker script configuration
- Output file generation (.elf, .hex, .bin)

## Usage

Each board's `CMakeLists.txt` includes the module and calls the generation function:

```cmake
# Generate STM32CubeIDE project files
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32cubeide.cmake)
generate_stm32cubeide_project(
    PROJECT_NAME "${CMAKE_PROJECT_NAME}"
    MCU_NAME "STM32U575ZITxQ"
    MCU_FAMILY "STM32U5"
    MCU_CORE "thumb"
    FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv5-sp-d16"
    FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard"
    LINKER_SCRIPT "STM32U575xx_FLASH.ld"
    INCLUDE_DIRS "Core/Inc" "Drivers/STM32U5xx_HAL_Driver/Inc" ...
    SOURCE_DIRS "Core/Src" "Core/Inc"
    HAL_DEFINES "USE_HAL_DRIVER" "STM32U575xx"
)
```

## Function Parameters

- `PROJECT_NAME` - Name of the project (matches board directory)
- `MCU_NAME` - Specific STM32 MCU part number (e.g., STM32U575ZITxQ)
- `MCU_FAMILY` - STM32 family (e.g., STM32U5, STM32G4, STM32L4)
- `MCU_CORE` - Target architecture (usually "thumb" for ARM Cortex-M)
- `FPU_TYPE` - STM32CubeIDE FPU enumerated value (fpv4-sp-d16, fpv5-sp-d16)
- `FLOAT_ABI` - STM32CubeIDE float ABI enumerated value (hard, soft, softfp)
- `LINKER_SCRIPT` - Linker script filename
- `INCLUDE_DIRS` - List of include directories (relative to board directory)
- `SOURCE_DIRS` - List of source directories (relative to board directory)
- `HAL_DEFINES` - List of preprocessor defines (USE_HAL_DRIVER, STM32xxx)

## Importing into STM32CubeIDE

1. Run CMake configuration on any board:
   ```bash
   cd boards/nucleo-U575ZI-Q
   cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
   ```

2. Open STM32CubeIDE

3. Import the project:
   - File → Import → General → Existing Projects into Workspace
   - Browse to the board directory (e.g., `boards/nucleo-U575ZI-Q`)
   - Select the project and click Finish

## Features

- **CMake-first approach** - Preserves your existing CMake build system
- **External build integration** - Uses CMake/Ninja instead of managed build (resolves FreeRTOS build errors)
- **Automatic path resolution** - Include and source paths are automatically configured
- **Cross-platform** - Works with CMake on Windows, Linux, and macOS
- **Multi-board support** - Each board generates its own project files
- **STM32CubeCLT integration** - Uses STM32CubeCLT_1.18.0 toolchain for consistent builds
- **Version controlled templates** - Template files are tracked in git
- **Auto-generated files** - Project files are gitignored and regenerated on each CMake run

## Board Support

Currently configured boards:

- `boards/nucleo-U575ZI-Q` - STM32U575ZI Cortex-M33
- `boards/custom-G474` - STM32G474 Cortex-M4F
- `boards/nucleo-L476RG` - STM32L476RG Cortex-M4F

## Template Customization

Templates are located in `cmake/templates/`:

- `project.in` - Eclipse project descriptor template
- `language.settings.xml.in` - Compiler settings template
- `org.eclipse.cdt.core.prefs.in` - CDT preferences template

Variables are substituted using CMake's `configure_file()` with `@VARIABLE@` syntax.

## Benefits

- **Unified workflow** - Use CMake for building, STM32CubeIDE for debugging
- **Resolved build issues** - External builder eliminates "make -j2 all" FreeRTOS errors
- **Code completion** - Full IntelliSense support in STM32CubeIDE
- **Hardware debugging** - ST-Link debugging with breakpoints and watch windows
- **Peripheral views** - STM32CubeIDE's register and memory views
- **No duplication** - Single source of truth in CMake files
- **Consistent toolchain** - Same STM32CubeCLT used for both command line and IDE builds