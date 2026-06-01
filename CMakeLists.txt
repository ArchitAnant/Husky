cmake_minimum_required(VERSION 3.15)

# Include Pico SDK
include(pico_sdk_import.cmake OPTIONAL)

project(my_riscv_os LANGUAGES C ASM CXX)

if (COMMAND pico_sdk_init)
    pico_sdk_init()
endif()

# Output executables to the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Recursively find all C and Assembly source files in the src/ directory
file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.S")

add_executable(os_elf ${SRC_FILES})

# Include directories
target_include_directories(os_elf PRIVATE src)

# Link against pico_stdlib
if (COMMAND pico_sdk_init)
    target_link_libraries(os_elf pico_stdlib)
    pico_enable_stdio_usb(os_elf 0)
    pico_enable_stdio_uart(os_elf 1)
    pico_add_extra_outputs(os_elf)
endif()

