cmake_minimum_required(VERSION 3.13)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

project(PicoOS C ASM)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb -O1 -nostdlib -ffreestanding -Wall -Wextra")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_EXE_LINKER_FLAGS "-T ${CMAKE_SOURCE_DIR}/pico.ld -nostartfiles -Wl,--gc-sections")

include_directories(include)

add_executable(os.elf
    arch/startup.S
    arch/port.c
    kernel/os.c
    kernel/heap.c
    kernel/sync.c
    kernel/string_utils.c
    drivers/uart.c
    drivers/clocks.c
    drivers/gpio.c
    app/main.c
    app/shell.c
    app/tasks.c
)

target_link_libraries(os.elf gcc)
set_target_properties(os.elf PROPERTIES SUFFIX "")

# elf to bin
add_custom_command(TARGET os.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary os.elf os.bin
    COMMENT "Generating os.bin..."
)
# uf for ram
add_custom_command(TARGET os.elf POST_BUILD
    COMMAND python ${CMAKE_SOURCE_DIR}/make_uf2.py ${CMAKE_CURRENT_BINARY_DIR}/os.bin ${CMAKE_CURRENT_BINARY_DIR}/os.uf2
    COMMENT "Generating os.uf2 for RAM..."
)