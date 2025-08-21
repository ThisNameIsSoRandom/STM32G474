# STM32CubeIDE Integration Module
# Generates project files for importing CMake projects into STM32CubeIDE
#
# Usage:
#   include(cmake/stm32cubeide.cmake)
#   generate_stm32cubeide_project(
#       PROJECT_NAME "nucleo-U575ZI-Q"
#       MCU_NAME "STM32U575ZITxQ"
#       MCU_FAMILY "STM32U5"
#       LINKER_SCRIPT "STM32U575xx_FLASH.ld"
#       INCLUDE_DIRS Core/Inc app/Inc Drivers/STM32U5xx_HAL_Driver/Inc
#       SOURCE_DIRS Core/Src app/Src
#   )

function(generate_stm32cubeide_project)
    set(options)
    set(oneValueArgs PROJECT_NAME PROJECT_TYPE MCU_NAME MCU_FAMILY MCU_CORE FPU_TYPE FLOAT_ABI LINKER_SCRIPT)
    set(multiValueArgs INCLUDE_DIRS SOURCE_DIRS HAL_DEFINES)
    cmake_parse_arguments(CUBE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set default PROJECT_TYPE if not specified
    if(NOT CUBE_PROJECT_TYPE)
        set(CUBE_PROJECT_TYPE "EXECUTABLE")
    endif()
    
    # Validate required arguments
    if(NOT CUBE_PROJECT_NAME)
        message(FATAL_ERROR "generate_stm32cubeide_project: PROJECT_NAME is required")
    endif()
    
    # Validate MCU-specific arguments only for EXECUTABLE projects
    if(CUBE_PROJECT_TYPE STREQUAL "EXECUTABLE")
        if(NOT CUBE_MCU_NAME)
            message(FATAL_ERROR "generate_stm32cubeide_project: MCU_NAME is required for EXECUTABLE projects")
        endif()
        if(NOT CUBE_MCU_FAMILY)
            message(FATAL_ERROR "generate_stm32cubeide_project: MCU_FAMILY is required for EXECUTABLE projects")
        endif()
        if(NOT CUBE_MCU_CORE)
            message(FATAL_ERROR "generate_stm32cubeide_project: MCU_CORE is required for EXECUTABLE projects")
        endif()
        if(NOT CUBE_FPU_TYPE)
            message(FATAL_ERROR "generate_stm32cubeide_project: FPU_TYPE is required for EXECUTABLE projects")
        endif()
        if(NOT CUBE_FLOAT_ABI)
            message(FATAL_ERROR "generate_stm32cubeide_project: FLOAT_ABI is required for EXECUTABLE projects")
        endif()
    elseif(CUBE_PROJECT_TYPE STREQUAL "LIBRARY")
        # Set generic defaults for library projects
        if(NOT CUBE_MCU_NAME)
            set(CUBE_MCU_NAME "Generic_ARM")
        endif()
        if(NOT CUBE_MCU_FAMILY)
            set(CUBE_MCU_FAMILY "ARM")
        endif()
        if(NOT CUBE_MCU_CORE)
            set(CUBE_MCU_CORE "thumb")
        endif()
        if(NOT CUBE_FPU_TYPE)
            set(CUBE_FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv4-sp-d16")
        endif()
        if(NOT CUBE_FLOAT_ABI)
            set(CUBE_FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard")
        endif()
        if(NOT CUBE_LINKER_SCRIPT)
            set(CUBE_LINKER_SCRIPT "library.ld")
        endif()
    else()
        message(FATAL_ERROR "generate_stm32cubeide_project: PROJECT_TYPE must be either EXECUTABLE or LIBRARY")
    endif()
    
    # Set template variables
    set(PROJECT_NAME "${CUBE_PROJECT_NAME}")
    set(MCU_NAME "${CUBE_MCU_NAME}")
    set(MCU_FAMILY "${CUBE_MCU_FAMILY}")
    set(MCU_CORE "${CUBE_MCU_CORE}")
    set(FPU_TYPE "${CUBE_FPU_TYPE}")
    set(FLOAT_ABI "${CUBE_FLOAT_ABI}")
    set(LINKER_SCRIPT "${CUBE_LINKER_SCRIPT}")
    
    # Convert STM32CubeIDE enumerated values to GCC flag values for linker
    if(FPU_TYPE MATCHES "fpv4-sp-d16")
        set(FPU_GCC_FLAG "fpv4-sp-d16")
    elseif(FPU_TYPE MATCHES "fpv5-sp-d16")
        set(FPU_GCC_FLAG "fpv5-sp-d16")
    else()
        set(FPU_GCC_FLAG "fpv4-sp-d16")  # Default fallback
    endif()
    
    if(FLOAT_ABI MATCHES "hard")
        set(FLOAT_ABI_GCC_FLAG "hard")
    elseif(FLOAT_ABI MATCHES "soft")
        set(FLOAT_ABI_GCC_FLAG "soft")
    elseif(FLOAT_ABI MATCHES "softfp")
        set(FLOAT_ABI_GCC_FLAG "softfp")
    else()
        set(FLOAT_ABI_GCC_FLAG "hard")  # Default fallback
    endif()
    
    # Generate unique IDs for .cproject file
    string(TIMESTAMP UNIQUE_ID "%Y%m%d%H%M%S")
    string(RANDOM LENGTH 8 ALPHABET "0123456789" RANDOM_SUFFIX)
    set(PROJECT_CONFIG_ID "${UNIQUE_ID}${RANDOM_SUFFIX}")
    set(PROJECT_CONFIG_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}1")
    set(PROJECT_TARGET_ID "${UNIQUE_ID}${RANDOM_SUFFIX}2")
    set(TOOLCHAIN_ID "${UNIQUE_ID}${RANDOM_SUFFIX}3")
    set(TOOLCHAIN_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}4")
    
    # Tool IDs
    set(ASM_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}10")
    set(C_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}11")
    set(CPP_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}12")
    set(LINK_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}13")
    set(HEX_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}14")
    set(BIN_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}15")
    set(SIZE_TOOL_ID "${UNIQUE_ID}${RANDOM_SUFFIX}16")
    
    # Release tool IDs
    set(ASM_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}20")
    set(C_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}21")
    set(CPP_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}22")
    set(LINK_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}23")
    set(HEX_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}24")
    set(BIN_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}25")
    set(SIZE_TOOL_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}26")
    
    # Option IDs (Debug)
    set(MCU_OPTION_ID "${UNIQUE_ID}${RANDOM_SUFFIX}30")
    set(CPU_OPTION_ID "${UNIQUE_ID}${RANDOM_SUFFIX}31")
    set(CORE_OPTION_ID "${UNIQUE_ID}${RANDOM_SUFFIX}32")
    set(FPU_OPTION_ID "${UNIQUE_ID}${RANDOM_SUFFIX}33")
    set(FLOATABI_OPTION_ID "${UNIQUE_ID}${RANDOM_SUFFIX}34")
    set(PLATFORM_ID "${UNIQUE_ID}${RANDOM_SUFFIX}35")
    set(BUILDER_ID "${UNIQUE_ID}${RANDOM_SUFFIX}36")
    
    # Option IDs (Release)
    set(MCU_OPTION_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}40")
    set(CPU_OPTION_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}41")
    set(CORE_OPTION_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}42")
    set(FPU_OPTION_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}43")
    set(FLOATABI_OPTION_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}44")
    set(PLATFORM_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}45")
    set(BUILDER_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}46")
    
    # More option IDs for each tool
    set(ASM_DEBUG_ID "${UNIQUE_ID}${RANDOM_SUFFIX}50")
    set(ASM_DEFINES_ID "${UNIQUE_ID}${RANDOM_SUFFIX}51")
    set(ASM_INC_ID "${UNIQUE_ID}${RANDOM_SUFFIX}52")
    set(ASM_INPUT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}53")
    
    set(C_DEBUG_ID "${UNIQUE_ID}${RANDOM_SUFFIX}60")
    set(C_OPT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}61")
    set(C_DEFINES_ID "${UNIQUE_ID}${RANDOM_SUFFIX}62")
    set(C_INC_ID "${UNIQUE_ID}${RANDOM_SUFFIX}63")
    set(C_MISC_ID "${UNIQUE_ID}${RANDOM_SUFFIX}64")
    set(C_INPUT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}65")
    
    set(CPP_DEBUG_ID "${UNIQUE_ID}${RANDOM_SUFFIX}70")
    set(CPP_OPT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}71")
    set(CPP_DEFINES_ID "${UNIQUE_ID}${RANDOM_SUFFIX}72")
    set(CPP_INC_ID "${UNIQUE_ID}${RANDOM_SUFFIX}73")
    set(CPP_MISC_ID "${UNIQUE_ID}${RANDOM_SUFFIX}74")
    set(CPP_INPUT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}75")
    
    set(LINK_SCRIPT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}80")
    set(LINK_LIBS_ID "${UNIQUE_ID}${RANDOM_SUFFIX}81")
    set(LINK_DIRS_ID "${UNIQUE_ID}${RANDOM_SUFFIX}82")
    set(LINK_OTHER_ID "${UNIQUE_ID}${RANDOM_SUFFIX}83")
    set(LINK_WARN_ID "${UNIQUE_ID}${RANDOM_SUFFIX}84")
    set(LINK_INPUT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}85")
    
    set(HEX_FORMAT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}90")
    set(BIN_FORMAT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}91")
    set(SIZE_FORMAT_ID "${UNIQUE_ID}${RANDOM_SUFFIX}92")
    
    # Release versions
    set(ASM_DEBUG_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}100")
    set(ASM_DEFINES_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}101")
    set(ASM_INC_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}102")
    set(ASM_INPUT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}103")
    
    set(C_DEBUG_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}110")
    set(C_OPT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}111")
    set(C_DEFINES_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}112")
    set(C_INC_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}113")
    set(C_MISC_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}114")
    set(C_INPUT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}115")
    
    set(CPP_DEBUG_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}120")
    set(CPP_OPT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}121")
    set(CPP_DEFINES_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}122")
    set(CPP_INC_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}123")
    set(CPP_MISC_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}124")
    set(CPP_INPUT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}125")
    
    set(LINK_SCRIPT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}130")
    set(LINK_LIBS_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}131")
    set(LINK_DIRS_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}132")
    set(LINK_OTHER_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}133")
    set(LINK_WARN_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}134")
    set(LINK_INPUT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}135")
    
    set(HEX_FORMAT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}140")
    set(BIN_FORMAT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}141")
    set(SIZE_FORMAT_ID_REL "${UNIQUE_ID}${RANDOM_SUFFIX}142")
    
    # Process HAL defines
    set(HAL_DEFINES_C "")
    set(HAL_DEFINES_CPP "")
    set(HAL_DEFINES_ASM "")
    
    # Default HAL defines if none provided
    if(NOT CUBE_HAL_DEFINES)
        set(CUBE_HAL_DEFINES "USE_HAL_DRIVER" "${MCU_FAMILY}xx")
    endif()
    
    foreach(define ${CUBE_HAL_DEFINES})
        set(HAL_DEFINES_C "${HAL_DEFINES_C}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${define}\"/>\n")
        set(HAL_DEFINES_CPP "${HAL_DEFINES_CPP}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${define}\"/>\n")
        set(HAL_DEFINES_ASM "${HAL_DEFINES_ASM}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"${define}\"/>\n")
    endforeach()
    
    # Process include directories - convert to XML entries for .cproject
    set(INCLUDE_PATHS_C "")
    set(INCLUDE_PATHS_CPP "")
    set(INCLUDE_PATHS_ASM "")
    
    foreach(inc_dir ${CUBE_INCLUDE_DIRS})
        # Convert relative paths to project-relative
        get_filename_component(abs_path "${CMAKE_CURRENT_SOURCE_DIR}/${inc_dir}" ABSOLUTE)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${abs_path}")
        set(INCLUDE_PATHS_C "${INCLUDE_PATHS_C}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${rel_path}}&quot;\"/>\n")
        set(INCLUDE_PATHS_CPP "${INCLUDE_PATHS_CPP}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${rel_path}}&quot;\"/>\n")
        set(INCLUDE_PATHS_ASM "${INCLUDE_PATHS_ASM}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${rel_path}}&quot;\"/>\n")
    endforeach()
    
    # Add app include paths
    file(RELATIVE_PATH app_rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/app")
    set(INCLUDE_PATHS_C "${INCLUDE_PATHS_C}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_C "${INCLUDE_PATHS_C}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/RTT/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_C "${INCLUDE_PATHS_C}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/Tasks/Inc}&quot;\"/>\n")
    
    set(INCLUDE_PATHS_CPP "${INCLUDE_PATHS_CPP}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_CPP "${INCLUDE_PATHS_CPP}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/RTT/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_CPP "${INCLUDE_PATHS_CPP}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/Tasks/Inc}&quot;\"/>\n")
    
    set(INCLUDE_PATHS_ASM "${INCLUDE_PATHS_ASM}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_ASM "${INCLUDE_PATHS_ASM}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/RTT/Inc}&quot;\"/>\n")
    set(INCLUDE_PATHS_ASM "${INCLUDE_PATHS_ASM}\t\t\t\t\t<listOptionValue builtIn=\"false\" value=\"&quot;\${workspace_loc:/${PROJECT_NAME}/${app_rel_path}/Src/Tasks/Inc}&quot;\"/>\n")
    
    # Process source directories for .project file
    set(SOURCE_PATHS "")
    foreach(src_dir ${CUBE_SOURCE_DIRS})
        get_filename_component(abs_path "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}" ABSOLUTE)
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${abs_path}")
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>${rel_path}</name><type>2</type><location>PROJECT_LOC/${rel_path}</location></link>\n")
    endforeach()
    
    # Add library-specific resources for app projects
    if(CUBE_PROJECT_TYPE STREQUAL "LIBRARY")
        # For library projects (app itself), add component directories (Inc/Src already added by SOURCE_DIRS)
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>BQ40Z80</name><type>2</type><location>PROJECT_LOC/Src/BQ40Z80</location></link>\n")
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>Tasks</name><type>2</type><location>PROJECT_LOC/Src/Tasks</location></link>\n")
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>RTT</name><type>2</type><location>PROJECT_LOC/Src/RTT</location></link>\n")
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>tests</name><type>2</type><location>PROJECT_LOC/tests</location></link>\n")
        set(SOURCE_PATHS "${SOURCE_PATHS}\t\t<link><name>uTests</name><type>2</type><location>PROJECT_LOC/uTests</location></link>\n")
    endif()
    
    # Create .settings directory if it doesn't exist
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.settings")
    
    # Get the project root directory (where cmake/ folder is located)
    # Try different paths depending on project structure
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
        get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    else()
        message(FATAL_ERROR "Cannot find cmake/ directory. Expected at ../cmake or ../../cmake")
    endif()
    
    # Configure and generate .project file
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/project.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.project"
        @ONLY
    )
    
    # Configure and generate .cproject file
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/cproject.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.cproject"
        @ONLY
    )
    
    # Configure and generate .settings/language.settings.xml
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/language.settings.xml.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.settings/language.settings.xml"
        @ONLY
    )
    
    # Configure and generate .settings/org.eclipse.cdt.core.prefs
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/org.eclipse.cdt.core.prefs.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/.settings/org.eclipse.cdt.core.prefs"
        @ONLY
    )
    
    # Configure and generate Makefile wrapper for STM32CubeIDE
    configure_file(
        "${PROJECT_ROOT}/cmake/templates/Makefile.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/Makefile"
        @ONLY
    )
    
    message(STATUS "Generated STM32CubeIDE project files for ${PROJECT_NAME}")
    message(STATUS "  MCU: ${MCU_NAME} (${MCU_FAMILY})")
    message(STATUS "  Linker script: ${LINKER_SCRIPT}")
    message(STATUS "  Import into STM32CubeIDE: File -> Import -> General -> Existing Projects into Workspace")
    
endfunction()