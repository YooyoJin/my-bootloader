# 交叉工具链: arm-none-eabi-gcc (Cortex-M4, 目标HC32F460)
# 路径查找顺序: 环境变量 ARMGCC_PATH > 已知安装路径 > 裸PATH
if(DEFINED ENV{ARMGCC_PATH})
    set(ARMGCC_BIN "$ENV{ARMGCC_PATH}")
elseif(EXISTS "E:/Program Files/arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi/bin")
    set(ARMGCC_BIN "E:/Program Files/arm-gnu-toolchain-13.2.Rel1-mingw-w64-i686-arm-none-eabi/bin")
endif()

# 关键: 工具链bin目录前置到PATH, 编译器只写"名字"。
# 若把含空格的完整路径直接写给CMAKE_C_COMPILER, cmake的存在性检查会误报
set(ENV{PATH} "${ARMGCC_BIN};$ENV{PATH}")

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR         arm-none-eabi-ar)
set(CMAKE_RANLIB     arm-none-eabi-ranlib)
set(CMAKE_OBJCOPY    arm-none-eabi-objcopy CACHE FILEPATH "objcopy")
set(CMAKE_SIZE       arm-none-eabi-size    CACHE FILEPATH "size")

# 无目标硬件: cmake试探编译改为静态库, 避免链接可执行文件失败
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 交叉编译查找隔离: 不在宿主机找程序/库
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# 构建驱动: 交叉编译时cmake无法从arm编译器目录推断make, 显式用本机mingw32-make
find_program(MINGW_MAKE_TOOL
    NAMES mingw32-make
    HINTS "E:/Program Files/mingw64/bin"
    PATHS ENV PATH)
if(MINGW_MAKE_TOOL)
    set(CMAKE_MAKE_PROGRAM ${MINGW_MAKE_TOOL} CACHE FILEPATH "make program" FORCE)
endif()