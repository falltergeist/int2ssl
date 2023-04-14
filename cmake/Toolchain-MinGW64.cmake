# Toolchain file to cross compile on Linux targeting Windows (x64/mingw-w64).
# Running:
# cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/Toolchain-MinGW64.cmake ....

set(CMAKE_SYSTEM_NAME Windows)

set(MINGW_COMPILER_PREFIX "x86_64-w64-mingw32" CACHE STRING "What compiler prefix to use for mingw (i686-w64-mingw32 or x86_64-w64-mingw32)")
set(MINGW_SYSROOT "/usr/${MINGW_COMPILER_PREFIX}" CACHE STRING "What sysroot to use for mingw")

# Which compilers to use for C and C++
find_program(CMAKE_C_COMPILER NAMES ${MINGW_COMPILER_PREFIX}-gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${MINGW_COMPILER_PREFIX}-g++)
find_program(CMAKE_RC_COMPILER NAMES ${MINGW_COMPILER_PREFIX}-windres)

set(CMAKE_FIND_ROOT_PATH ${MINGW_SYSROOT})

# Adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
