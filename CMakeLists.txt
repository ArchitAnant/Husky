cmake_minimum_required(VERSION 3.15)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

set(CMAKE_C_COMPILER riscv64-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER riscv64-unknown-elf-gcc)

project(my_riscv_os LANGUAGES C ASM)

# Output executables to the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Recursively find all C and Assembly source files in the src/ directory
file(GLOB_RECURSE SRC_FILES "src/*.c" "src/*.S")

add_executable(os.elf ${SRC_FILES})

# Include directories
target_include_directories(os.elf PRIVATE src)

# Compiler options
target_compile_options(os.elf PRIVATE
    -Wall
    -Wextra
    -march=rv64gc
    -mabi=lp64d
    -mcmodel=medany
    -ffreestanding
    -nostdlib
)

# Linker options
target_link_options(os.elf PRIVATE
    -nostartfiles
    -nostdlib
    -T${CMAKE_CURRENT_SOURCE_DIR}/src/lds/virt.lds
    -Wl,--no-warn-rwx-segments
)

# Custom target to run the OS in QEMU
add_custom_target(run
    COMMAND qemu-system-riscv64 -machine virt -cpu rv64 -smp 4 -m 128M -nographic -serial mon:stdio -bios none -kernel os.elf
    DEPENDS os.elf
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    USES_TERMINAL
)
