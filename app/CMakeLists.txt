cmake_minimum_required(VERSION 3.16)

project(app
        VERSION 0.0.0
        LANGUAGES C CXX ASM
        DESCRIPTION "Application library with canonical structure"
)

# Create main app static library
add_library(${PROJECT_NAME} STATIC)

# Add main app source files
target_sources(${PROJECT_NAME} PRIVATE
    Src/placeholder.cpp
    Src/logging.cpp
)

# Public includes (visible to users of the library)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# Private includes (only visible within the library)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Inc
)

# Add subdirectories for RTT and Tasks
add_subdirectory(Src/RTT)
add_subdirectory(Src/Tasks)

# Link with subdirectory libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    RTT
    Tasks
)

# Inherit compile options from parent project
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${STM32CUBEMX_C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${STM32CUBEMX_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${STM32CUBEMX_ASM_FLAGS}>
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# C++ files are now native .cpp - no forced compilation needed

# Create alias for find_package compatibility
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Export targets to build directory for immediate use
export(TARGETS ${PROJECT_NAME} RTT Tasks
    NAMESPACE ${PROJECT_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)

# Create a simple config file in build directory
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake\")
set(${PROJECT_NAME}_FOUND TRUE)
")