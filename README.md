# 🛰️ Bare-Metal RISC-V Operating System
**A custom OS kernel built from scratch in C for the RISC-V architecture (Ported to Raspberry Pi Pico 2).**

## 📌 Project Overview
This project is a journey into the "heart of the machine." I am developing a bare-metal kernel targeting the **Raspberry Pi Pico 2 (RP2350 Hazard3 RISC-V core)**. The goal is to understand kernel-level memory management, process scheduling, and hardware-software interfacing without the abstraction of a standard C library (while leveraging the Pico SDK for low-level board initialization).

## 🛠️ Tech Stack
- **Language:** C, RISC-V Assembly
- **Environment:** Raspberry Pi Pico 2 (RP2350)
- **Toolchain:** `riscv64-unknown-elf-gcc`, `cmake`, `pico-sdk`

## 🚀 Key Features (In Development)
- [x] **Bootloader Logic:** RP2350 bootrom via `pico-sdk` to C execution.
- [x] **UART Driver:** Basic I/O for serial console communication.
- [x] **Memory Management:** Implementing a flat Page Allocator and `kmalloc` array across SRAM.
- [ ] **Interrupt Handling:** Managing hardware traps for the RISC-V architecture.

## 📂 Project Structure
- `src/kernel.c`: Main kernel logic, shell runner, and memory tests.
- `src/mm/`: Physical page allocator and chunk memory manager.
- `src/trap/`: Exception and interrupt vector table handling.
- `src/uart/`: Abstracted UART handling mapping to Pico SDK stdio.
- `CMakeLists.txt`: Configures the project targeting the Pico SDK.

## 💻 How to Build and Run
1. Ensure you have the RISC-V toolchain (`riscv64-unknown-elf-gcc`), CMake, and the **Raspberry Pi Pico SDK** installed.
2. Clone the repo: `git clone https://github.com/Aritrahutait07/my_riscv_os`
3. Configure the OS for the Pico 2 RISC-V cores:
   ```bash
   export PICO_SDK_PATH="/path/to/pico-sdk"
   cmake -B build -DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350-riscv -DPICO_GCC_TRIPLE=riscv64-unknown-elf
   ```
4. Build the OS:
   ```bash
   cmake --build build
   ```
5. Deploy to hardware:
   - Hold the **BOOTSEL** button on your Pico 2 and plug it into your computer via USB.
   - Drag and drop `build/os_elf.uf2` into the `RPI-RP2` drive that appears.
   - Open a serial monitor to interact with the OS.

## 🧪 Testing and Debugging
- build the uf2 file and flash it to the Pico 2.
- Use a serial terminal (like `minicom` or `PuTTY`) to connect to the Pico 2's UART output for debugging and interaction.
