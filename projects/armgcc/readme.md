# armgcc工程——bootloader固件(HC32F460)

## 目录职责

- 本目录只放构建相关文件: `CMakeLists.txt`、`toolchain.cmake`、`build.bat`、newlib桩`syscalls.c`
- 源码在仓库根`source/`下: app(组装根+跳转业务) / port(HC32F460移植层) / modules(纯逻辑模块) / vendor(芯片SDK+RTT, 原样不修改)

## 构建

- VSCode: 直接F7(CMake Tools), settings.json已绑定本目录与toolchain
- 命令行: 运行`build.bat`, 或手动

```bat
cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake
cmake --build build
```

- 产物在`build/`下: `bootloader.elf/.hex/.bin/.map`
- clangd索引: 编译数据库在`build/compile_commands.json`, VSCode settings.json的
  `--compile-commands-dir`已指向该目录, 构建一次后即可跳转

## 工具链

- arm-none-eabi-gcc 13.2 (路径优先级: 环境变量`ARMGCC_PATH` > toolchain.cmake内置路径)
- make: mingw32-make (E:/Program Files/mingw64/bin)

## 注意

- vendor下的hc32l110/hc32l170暂时保留在仓库中, 但不参与本工程构建
- APP起始地址`BOOT_APP_START_ADDR`(0x4000)定义在`source/app/jumpapp.h`
