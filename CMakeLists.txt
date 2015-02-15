set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (by default Debug)")

cmake_minimum_required(VERSION 2.8)

project(int2ssl)

set(SOURCES main.cpp
Hacks/CArchive.h
Hacks/CArchive.cpp
Hacks/CFile.h
Hacks/CFile.cpp
Hacks/CMap.h
Namespace.h
Namespace.cpp
Node.h
Node.cpp
ObjectAttributes.h
Opcode.h
ProcTable.h
ProcTable.cpp
Utility.h
Utility.cpp
FalloutScript.h
FalloutScript.cpp
FalloutScriptDecompile.cpp
FalloutScriptDefObject.cpp
FalloutScriptDump.cpp
FalloutScriptStore.cpp
OpcodeAttributes.cpp
Opcode.cpp
StartupCode.h
StartupCode.cpp
XGetopt.h
XGetopt.cpp
)

add_definitions (-Wall)

add_executable(int2ssl ${SOURCES})
