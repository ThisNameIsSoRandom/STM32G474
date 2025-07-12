# App static library - board-agnostic implementation
# This library can be built for different target boards and platforms

# Determine the target board and platform from parent scope
if(NOT DEFINED TARGET_BOARD)
    message(FATAL_ERROR "TARGET_BOARD not defined. Must be set by parent CMakeLists.txt")
endif()

if(NOT DEFINED TARGET_PLATFORM)
    set(TARGET_PLATFORM "default")
endif()

# Create app static library with unique name per platform
set(APP_LIB_NAME "app_${TARGET_BOARD}_${TARGET_PLATFORM}")
add_library(${APP_LIB_NAME} STATIC
    app_tasks.c
)

# Add include directories
target_include_directories(${APP_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Board-specific configurations
if(TARGET_BOARD STREQUAL "STM32G474")
    target_compile_definitions(${APP_LIB_NAME} PUBLIC
        USE_HAL_DRIVER
        STM32G474xx
    )
    # Link against G474 libraries
    target_link_libraries(${APP_LIB_NAME} PUBLIC
        stm32cubemx
    )
    
elseif(TARGET_BOARD STREQUAL "STM32H755" AND TARGET_PLATFORM STREQUAL "CM7")
    target_compile_definitions(${APP_LIB_NAME} PUBLIC
        USE_HAL_DRIVER
        STM32H755xx
        CORE_CM7
        USE_NUCLEO_64
    )
    # Link against H755 CM7 libraries
    target_link_libraries(${APP_LIB_NAME} PUBLIC
        stm32cubemx_cm7
    )
    
elseif(TARGET_BOARD STREQUAL "STM32H755" AND TARGET_PLATFORM STREQUAL "CM4")
    target_compile_definitions(${APP_LIB_NAME} PUBLIC
        USE_HAL_DRIVER
        STM32H755xx
        CORE_CM4
        USE_NUCLEO_64
    )
    # Link against H755 CM4 libraries
    target_link_libraries(${APP_LIB_NAME} PUBLIC
        stm32cubemx_cm4
    )
    
else()
    message(FATAL_ERROR "Unsupported TARGET_BOARD/TARGET_PLATFORM combination: ${TARGET_BOARD}/${TARGET_PLATFORM}")
endif()

# Common compile definitions
target_compile_definitions(${APP_LIB_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
)

# Export the library name for parent CMakeLists.txt
set(APP_LIBRARY_NAME ${APP_LIB_NAME} PARENT_SCOPE)