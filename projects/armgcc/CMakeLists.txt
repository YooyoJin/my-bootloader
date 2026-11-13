cmake_minimum_required(VERSION 3.15)
project(my-bootloader C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../source)
set(VENDOR  ${SRC_ROOT}/vendor)
set(CHIP    hc32f460)

# ----------------------------------------------------------------------------
# MCU 编译/链接参数(Cortex-M4F, 与my-libs工程一致)
# ----------------------------------------------------------------------------
set(MCU_FLAGS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
add_compile_options(${MCU_FLAGS} -ffunction-sections -fdata-sections -g -O0)
add_compile_definitions(HC32F460 USE_DDL_DRIVER DEBUG)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -x assembler-with-cpp")

# 链接脚本: xE=512KB(PETB/PETG), xC=256KB(KCTA); 可被-DLINKER_SCRIPT覆盖
set(LINKER_SCRIPT ${VENDOR}/${CHIP}/linker/HC32F460xE.ld CACHE FILEPATH "linker script")
set(STARTUP_FILE  ${VENDOR}/${CHIP}/start/gcc/startup_hc32f460.S)

# ----------------------------------------------------------------------------
# 模块库登记处(与my-libs保持同步)
# ----------------------------------------------------------------------------
add_library(crc STATIC ${SRC_ROOT}/modules/crc/crc.c)
target_include_directories(crc PUBLIC ${SRC_ROOT}/modules/crc)
target_compile_options(crc PRIVATE -Wall -Wextra)

add_library(log STATIC ${SRC_ROOT}/modules/log/log.c)
target_include_directories(log PUBLIC ${SRC_ROOT}/modules/log)
target_compile_options(log PRIVATE -Wall -Wextra)

# ----------------------------------------------------------------------------
# 固件目标: app + port + RTT + 芯片SDK + 启动文件 + newlib桩
# 注意: vendor下的hc32l110/hc32l170暂保留在仓库但不参与构建
# ----------------------------------------------------------------------------
file(GLOB APP_SRC  ${SRC_ROOT}/app/*.c)
file(GLOB PORT_SRC ${SRC_ROOT}/port/${CHIP}/*.c)
file(GLOB RTT_SRC  ${VENDOR}/seggerrtt/*.c)
file(GLOB SDK_SRC  ${VENDOR}/${CHIP}/*.c ${VENDOR}/${CHIP}/hc32_ll_driver/*.c)

add_executable(bootloader
    ${APP_SRC}
    ${PORT_SRC}
    ${RTT_SRC}
    ${SDK_SRC}
    ${STARTUP_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/syscalls.c
)
set_target_properties(bootloader PROPERTIES SUFFIX ".elf")

target_include_directories(bootloader PRIVATE
    ${SRC_ROOT}/app
    ${SRC_ROOT}/common
    ${SRC_ROOT}/port/${CHIP}
    ${SRC_ROOT}/modules/crc
    ${SRC_ROOT}/modules/log
    ${VENDOR}/seggerrtt
    ${VENDOR}/${CHIP}
    ${VENDOR}/${CHIP}/cmsis
    ${VENDOR}/${CHIP}/hc32_ll_driver
)
target_link_libraries(bootloader PRIVATE crc log)
target_link_options(bootloader PRIVATE
    ${MCU_FLAGS}
    -T${LINKER_SCRIPT}
    --specs=nano.specs  # 精简版C标准库
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/bootloader.map
    -Wl,--no-warn-rwx-segments
)

# ----------------------------------------------------------------------------
# 后处理: 生成hex/bin + 打印size
# ----------------------------------------------------------------------------
add_custom_command(TARGET bootloader POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:bootloader> ${CMAKE_BINARY_DIR}/bootloader.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:bootloader> ${CMAKE_BINARY_DIR}/bootloader.bin
    COMMAND ${CMAKE_SIZE}    $<TARGET_FILE:bootloader>
    COMMENT "Generating hex/bin and print size"
)
