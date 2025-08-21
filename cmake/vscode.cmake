# VSCode Integration Module
# Generates VSCode configuration files for importing CMake projects into VSCode
#
# Usage:
#   include(cmake/vscode.cmake)
#   generate_vscode_project(
#       PROJECT_NAME "nucleo-U575ZI-Q"
#       PROJECT_TYPE "EXECUTABLE"
#       MCU_NAME "STM32U575ZITxQ"
#       MCU_FAMILY "STM32U5"
#       LINKER_SCRIPT "STM32U575xx_FLASH.ld"
#       INCLUDE_DIRS Core/Inc Drivers/STM32U5xx_HAL_Driver/Inc
#       SOURCE_DIRS Core/Src Core/Inc
#   )

function(generate_vscode_project)
    set(options)
    set(oneValueArgs PROJECT_NAME PROJECT_TYPE MCU_NAME MCU_FAMILY MCU_CORE FPU_TYPE FLOAT_ABI LINKER_SCRIPT)
    set(multiValueArgs INCLUDE_DIRS SOURCE_DIRS HAL_DEFINES)
    cmake_parse_arguments(VSCODE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set default PROJECT_TYPE if not specified
    if(NOT VSCODE_PROJECT_TYPE)
        set(VSCODE_PROJECT_TYPE "EXECUTABLE")
    endif()
    
    # Validate required arguments
    if(NOT VSCODE_PROJECT_NAME)
        message(FATAL_ERROR "generate_vscode_project: PROJECT_NAME is required")
    endif()
    
    # Validate MCU-specific arguments only for EXECUTABLE projects
    if(VSCODE_PROJECT_TYPE STREQUAL "EXECUTABLE")
        if(NOT VSCODE_MCU_NAME)
            message(FATAL_ERROR "generate_vscode_project: MCU_NAME is required for EXECUTABLE projects")
        endif()
        if(NOT VSCODE_MCU_FAMILY)
            message(FATAL_ERROR "generate_vscode_project: MCU_FAMILY is required for EXECUTABLE projects")
        endif()
    elseif(VSCODE_PROJECT_TYPE STREQUAL "LIBRARY")
        # Set generic defaults for library projects
        if(NOT VSCODE_MCU_NAME)
            set(VSCODE_MCU_NAME "Generic_ARM")
        endif()
        if(NOT VSCODE_MCU_FAMILY)
            set(VSCODE_MCU_FAMILY "ARM")
        endif()
        if(NOT VSCODE_MCU_CORE)
            set(VSCODE_MCU_CORE "thumb")
        endif()
        if(NOT VSCODE_LINKER_SCRIPT)
            set(VSCODE_LINKER_SCRIPT "library.ld")
        endif()
    else()
        message(FATAL_ERROR "generate_vscode_project: PROJECT_TYPE must be either EXECUTABLE or LIBRARY")
    endif()
    
    # Set template variables
    set(PROJECT_NAME "${VSCODE_PROJECT_NAME}")
    set(MCU_NAME "${VSCODE_MCU_NAME}")
    set(MCU_FAMILY "${VSCODE_MCU_FAMILY}")
    set(MCU_CORE "${VSCODE_MCU_CORE}")
    set(LINKER_SCRIPT "${VSCODE_LINKER_SCRIPT}")
    set(PROJECT_TYPE "${VSCODE_PROJECT_TYPE}")
    
    # Determine build target name based on project type
    if(PROJECT_TYPE STREQUAL "EXECUTABLE")
        set(BUILD_TARGET "${PROJECT_NAME}")
        set(EXECUTABLE_PATH "\${workspaceFolder}/cmake-build-debug/${PROJECT_NAME}.elf")
        set(BUILD_DIR "cmake-build-debug")
        set(CMAKE_BOARD_ARG "\"-DBOARD=${PROJECT_NAME}\"")
        set(EXCLUDE_DRIVERS_MIDDLEWARE "true")
        set(CMAKE_CONFIGURE_SETTINGS "\"CMAKE_BUILD_TYPE\": \"Debug\",\n        \"CMAKE_MAKE_PROGRAM\": \"@NINJA_PATH@\",\n        \"BOARD\": \"@PROJECT_NAME@\"")
    else()
        set(BUILD_TARGET "app")
        set(EXECUTABLE_PATH "")
        set(BUILD_DIR "build")
        set(CMAKE_BOARD_ARG "")
        set(EXCLUDE_DRIVERS_MIDDLEWARE "false")
        set(CMAKE_CONFIGURE_SETTINGS "\"CMAKE_BUILD_TYPE\": \"Debug\",\n        \"CMAKE_MAKE_PROGRAM\": \"@NINJA_PATH@\"")
    endif()
    
    # Set toolchain paths (STM32CubeCLT)
    set(CMAKE_PATH "C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe")
    set(NINJA_PATH "C:/ST/STM32CubeCLT_1.18.0/Ninja/bin/ninja.exe")
    set(GCC_PATH "C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe")
    set(GDB_PATH "C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe")
    set(OPENOCD_PATH "C:/ST/STM32CubeCLT_1.18.0/OpenOCD/bin/openocd.exe")
    set(PROGRAMMER_PATH "C:/ST/STM32CubeCLT_1.18.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe")
    
    # Set IntelliSense mode based on MCU family
    if(MCU_FAMILY MATCHES "STM32H7")
        set(INTELLISENSE_MODE "gcc-arm")
        set(TARGET_ARCH "cortex-m7")
        set(OPENOCD_TARGET "stm32h7x")
    elseif(MCU_FAMILY MATCHES "STM32U5")
        set(INTELLISENSE_MODE "gcc-arm")
        set(TARGET_ARCH "cortex-m33")
        set(OPENOCD_TARGET "stm32u5x")
    elseif(MCU_FAMILY MATCHES "STM32G4")
        set(INTELLISENSE_MODE "gcc-arm")
        set(TARGET_ARCH "cortex-m4")
        set(OPENOCD_TARGET "stm32g4x")
    elseif(MCU_FAMILY MATCHES "STM32L4")
        set(INTELLISENSE_MODE "gcc-arm")
        set(TARGET_ARCH "cortex-m4")
        set(OPENOCD_TARGET "stm32l4x")
    else()
        set(INTELLISENSE_MODE "gcc-arm")
        set(TARGET_ARCH "cortex-m4")
        set(OPENOCD_TARGET "stm32f4x")
    endif()
    
    # Process include directories for IntelliSense
    set(INCLUDE_PATHS_JSON "")
    set(first_include TRUE)
    
    foreach(inc_dir ${VSCODE_INCLUDE_DIRS})
        if(NOT first_include)
            set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON},\n")
        endif()
        set(first_include FALSE)
        
        # Convert relative paths to absolute for IntelliSense
        get_filename_component(abs_path "${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}" ABSOLUTE)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${abs_path}")
        set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON}            \"\${workspaceFolder}/${rel_path}\"")
    endforeach()
    
    # Add app include paths for board projects
    if(PROJECT_TYPE STREQUAL "EXECUTABLE")
        file(RELATIVE_PATH app_rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/app")
        if(NOT first_include)
            set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON},\n")
        endif()
        set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON}            \"\${workspaceFolder}/${app_rel_path}/Inc\",\n")
        set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON}            \"\${workspaceFolder}/${app_rel_path}/Src/RTT/Inc\",\n")
        set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON}            \"\${workspaceFolder}/${app_rel_path}/Src/Tasks/Inc\",\n")
        set(INCLUDE_PATHS_JSON "${INCLUDE_PATHS_JSON}            \"\${workspaceFolder}/${app_rel_path}/Src/BQ40Z80/Inc\"")
    endif()
    
    # Process HAL defines for IntelliSense
    set(DEFINES_JSON "")
    set(first_define TRUE)
    
    # Default HAL defines if none provided
    if(NOT VSCODE_HAL_DEFINES)
        set(VSCODE_HAL_DEFINES "USE_HAL_DRIVER" "${MCU_FAMILY}xx")
    endif()
    
    foreach(define ${VSCODE_HAL_DEFINES})
        if(NOT first_define)
            set(DEFINES_JSON "${DEFINES_JSON},\n")
        endif()
        set(first_define FALSE)
        set(DEFINES_JSON "${DEFINES_JSON}            \"${define}\"")
    endforeach()
    
    # Create .vscode directory if it doesn't exist
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.vscode")
    
    # Get the project root directory (where cmake/ folder is located)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    else()
        message(FATAL_ERROR "Cannot find cmake/ directory. Expected at ../cmake or ../../cmake")
    endif()
    
    # Configure and generate VSCode configuration files
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/vscode_settings.json.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/settings.json"
        @ONLY
    )
    
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/vscode_c_cpp_properties.json.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/c_cpp_properties.json"
        @ONLY
    )
    
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/vscode_extensions.json.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/extensions.json"
        @ONLY
    )
    
    # Generate launch.json and choose tasks template based on project type
    if(PROJECT_TYPE STREQUAL "EXECUTABLE")
        configure_file(
            "${PROJECT_ROOT}/cmake/templates/vscode_launch.json.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/launch.json"
            @ONLY
        )
        
        # Use the full tasks.json template for executable projects
        set(TASKS_TEMPLATE "vscode_tasks.json.in")
    else()
        # Use a simpler tasks.json template for library projects
        set(TASKS_TEMPLATE "vscode_tasks_lib.json.in")
    endif()
    
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/${TASKS_TEMPLATE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/tasks.json"
        @ONLY
    )
    
    message(STATUS "Generated VSCode project files for ${PROJECT_NAME}")
    message(STATUS "  Type: ${PROJECT_TYPE}")
    message(STATUS "  MCU: ${MCU_NAME} (${MCU_FAMILY})")
    if(PROJECT_TYPE STREQUAL "EXECUTABLE")
        message(STATUS "  Debug support: enabled")
    else()
        message(STATUS "  Debug support: disabled (library project)")
    endif()
    message(STATUS "  Open in VSCode: File -> Open Folder -> ${CMAKE_CURRENT_SOURCE_DIR}")
    
    # Generate or update workspace file immediately (no deferral needed)
    generate_vscode_workspace()
    
endfunction()

# Function to generate or update multi-root workspace file
function(generate_vscode_workspace)
    # Get the project root directory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    else()
        return() # Can't find project root, skip workspace generation
    endif()
    
    # Always try to generate workspace when called
    
    set(WORKSPACE_FILE "${PROJECT_ROOT}/STM32G474.code-workspace")
    
    # Scan for existing projects with .vscode folders
    set(WORKSPACE_FOLDERS "")
    set(BUILD_TASKS "")
    set(CONFIG_TASKS "")
    set(CLEAN_TASKS "")
    set(BUILD_DEPENDS "")
    set(CONFIG_DEPENDS "")
    set(CLEAN_DEPENDS "")
    set(folder_count 0)
    
    # Check for app project
    if(EXISTS "${PROJECT_ROOT}/app/.vscode")
        set(WORKSPACE_FOLDERS "${WORKSPACE_FOLDERS}        {\n            \"name\": \"app\",\n            \"path\": \"./app\"\n        }")
        
        # Add app build tasks
        set(BUILD_TASKS "${BUILD_TASKS}            {\n                \"label\": \"app: Build app\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"--build\", \"\${workspaceFolder}/app/build\",\n                    \"--target\", \"app\",\n                    \"-j\", \"10\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [\"$gcc\"],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/app\"\n                }\n            }")
        
        set(CONFIG_TASKS "${CONFIG_TASKS}            {\n                \"label\": \"app: Configure CMake\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"-B\", \"\${workspaceFolder}/app/build\",\n                    \"-G\", \"Ninja\",\n                    \"-DCMAKE_BUILD_TYPE=Debug\",\n                    \"-DCMAKE_MAKE_PROGRAM=C:/ST/STM32CubeCLT_1.18.0/Ninja/bin/ninja.exe\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/app\"\n                }\n            }")
        
        set(CLEAN_TASKS "${CLEAN_TASKS}            {\n                \"label\": \"app: Clean Build\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"--build\", \"\${workspaceFolder}/app/build\",\n                    \"--target\", \"clean\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/app\"\n                }\n            }")
        
        set(BUILD_DEPENDS "${BUILD_DEPENDS}\"app: Build app\"")
        set(CONFIG_DEPENDS "${CONFIG_DEPENDS}\"app: Configure CMake\"")
        set(CLEAN_DEPENDS "${CLEAN_DEPENDS}\"app: Clean Build\"")
        math(EXPR folder_count "${folder_count} + 1")
    endif()
    
    # Check for board projects
    file(GLOB board_dirs "${PROJECT_ROOT}/boards/*")
    foreach(board_dir ${board_dirs})
        if(IS_DIRECTORY "${board_dir}" AND EXISTS "${board_dir}/.vscode")
            get_filename_component(board_name "${board_dir}" NAME)
            
            # Add to folders list
            if(folder_count GREATER 0)
                set(WORKSPACE_FOLDERS "${WORKSPACE_FOLDERS},\n")
                set(BUILD_TASKS "${BUILD_TASKS},\n")
                set(CONFIG_TASKS "${CONFIG_TASKS},\n")
                set(CLEAN_TASKS "${CLEAN_TASKS},\n")
                set(BUILD_DEPENDS "${BUILD_DEPENDS},\n                    ")
                set(CONFIG_DEPENDS "${CONFIG_DEPENDS},\n                    ")
                set(CLEAN_DEPENDS "${CLEAN_DEPENDS},\n                    ")
            endif()
            set(WORKSPACE_FOLDERS "${WORKSPACE_FOLDERS}        {\n            \"name\": \"${board_name}\",\n            \"path\": \"./boards/${board_name}\"\n        }")
            
            # Determine project name and target based on board
            if("${board_name}" STREQUAL "custom-G474")
                set(PROJECT_TARGET "STM32G474")
            else()
                set(PROJECT_TARGET "${board_name}")
            endif()
            
            # Add board build tasks
            set(BUILD_TASKS "${BUILD_TASKS}            {\n                \"label\": \"${board_name}: Build ${PROJECT_TARGET}\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"--build\", \"\${workspaceFolder}/boards/${board_name}/cmake-build-debug\",\n                    \"--target\", \"${PROJECT_TARGET}\",\n                    \"-j\", \"10\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [\"$gcc\"],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/boards/${board_name}\"\n                }\n            }")
            
            set(CONFIG_TASKS "${CONFIG_TASKS}            {\n                \"label\": \"${board_name}: Configure CMake\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"-B\", \"\${workspaceFolder}/boards/${board_name}/cmake-build-debug\",\n                    \"-G\", \"Ninja\",\n                    \"-DCMAKE_BUILD_TYPE=Debug\",\n                    \"-DCMAKE_MAKE_PROGRAM=C:/ST/STM32CubeCLT_1.18.0/Ninja/bin/ninja.exe\",\n                    \"-DCMAKE_C_COMPILER=C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe\",\n                    \"-DCMAKE_CXX_COMPILER=C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-g++.exe\",\n                    \"-DBOARD=${board_name}\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/boards/${board_name}\"\n                }\n            }")
            
            set(CLEAN_TASKS "${CLEAN_TASKS}            {\n                \"label\": \"${board_name}: Clean Build\",\n                \"type\": \"shell\",\n                \"command\": \"C:/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe\",\n                \"args\": [\n                    \"--build\", \"\${workspaceFolder}/boards/${board_name}/cmake-build-debug\",\n                    \"--target\", \"clean\"\n                ],\n                \"group\": \"individual-build\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": [],\n                \"options\": {\n                    \"cwd\": \"\${workspaceFolder}/boards/${board_name}\"\n                }\n            }")
            
            set(BUILD_DEPENDS "${BUILD_DEPENDS}\"${board_name}: Build ${PROJECT_TARGET}\"")
            set(CONFIG_DEPENDS "${CONFIG_DEPENDS}\"${board_name}: Configure CMake\"")
            set(CLEAN_DEPENDS "${CLEAN_DEPENDS}\"${board_name}: Clean Build\"")
            math(EXPR folder_count "${folder_count} + 1")
        endif()
    endforeach()
    
    # Only generate workspace if we found projects
    if(folder_count GREATER 0)
        # Generate workspace content with tasks
        set(WORKSPACE_CONTENT "{\n    \"folders\": [\n${WORKSPACE_FOLDERS}\n    ],\n    \"settings\": {\n        \"cmake.preferredGenerators\": [\"Ninja\"],\n        \"cmake.configureOnOpen\": false,\n        \"cmake.autoSelectActiveFolder\": true,\n        \"cmake.buildTask\": true,\n        \"cmake.automaticReconfigure\": false,\n        \"files.exclude\": {\n            \"**/build\": true,\n            \"**/cmake-build-*\": true,\n            \"**/Drivers\": true,\n            \"**/Middlewares\": true,\n            \"**/.git\": true,\n            \"**/.DS_Store\": true\n        },\n        \"search.exclude\": {\n            \"**/build\": true,\n            \"**/cmake-build-*\": true,\n            \"**/Drivers\": true,\n            \"**/Middlewares\": true\n        }\n    },\n    \"tasks\": {\n        \"version\": \"2.0.0\",\n        \"tasks\": [\n            {\n                \"label\": \"Build All Projects\",\n                \"type\": \"shell\",\n                \"command\": \"echo\",\n                \"args\": [\"Building all projects...\"],\n                \"group\": {\n                    \"kind\": \"build\",\n                    \"isDefault\": true\n                },\n                \"dependsOn\": [\n                    ${BUILD_DEPENDS}\n                ],\n                \"dependsOrder\": \"parallel\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": []\n            },\n${BUILD_TASKS},\n            {\n                \"label\": \"Configure All Projects\",\n                \"type\": \"shell\",\n                \"command\": \"echo\",\n                \"args\": [\"Configuring all projects...\"],\n                \"group\": \"build\",\n                \"dependsOn\": [\n                    ${CONFIG_DEPENDS}\n                ],\n                \"dependsOrder\": \"parallel\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": []\n            },\n${CONFIG_TASKS},\n            {\n                \"label\": \"Clean All Projects\",\n                \"type\": \"shell\",\n                \"command\": \"echo\",\n                \"args\": [\"Cleaning all projects...\"],\n                \"group\": \"build\",\n                \"dependsOn\": [\n                    ${CLEAN_DEPENDS}\n                ],\n                \"dependsOrder\": \"parallel\",\n                \"presentation\": {\n                    \"echo\": true,\n                    \"reveal\": \"always\",\n                    \"focus\": false,\n                    \"panel\": \"shared\",\n                    \"showReuseMessage\": false,\n                    \"clear\": false\n                },\n                \"problemMatcher\": []\n            },\n${CLEAN_TASKS}\n        ]\n    },\n    \"extensions\": {\n        \"recommendations\": [\n            \"ms-vscode.cpptools\",\n            \"ms-vscode.cmake-tools\",\n            \"twxs.cmake\",\n            \"marus25.cortex-debug\"\n        ]\n    }\n}")
        
        # Write workspace file
        file(WRITE "${WORKSPACE_FILE}" "${WORKSPACE_CONTENT}")
        
        message(STATUS "Generated VSCode multi-root workspace: STM32G474.code-workspace")
        message(STATUS "  Found ${folder_count} project(s)")
        message(STATUS "  Added workspace-level build tasks")
        message(STATUS "  Open workspace: File -> Open Workspace from File -> STM32G474.code-workspace")
    endif()
endfunction()