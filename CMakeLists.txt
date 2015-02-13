set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (by default Debug)")

cmake_minimum_required(VERSION 2.8)

project(int2ssl)

set(SOURCES main.cpp
Hacks/Types.h
Hacks/CString.h
Hacks/CString.cpp
Hacks/CArray.h
Hacks/CArray.cpp
Hacks/CArchive.h
Hacks/CArchive.cpp
Hacks/CFile.h
Hacks/CFile.cpp
Hacks/CMap.h
Hacks/CMap.cpp
FalloutScript.h
Namespace.h
Node.h
ObjectAttributes.h
Opcode.h
ProcTable.h
StartupCode.h
stdafx.h
Utility.h
XGetopt.h
FalloutScript.cpp
FalloutScriptDecompile.cpp
FalloutScriptDefObject.cpp
FalloutScriptDump.cpp
FalloutScriptStore.cpp
Namespace.cpp
Node.cpp
OpcodeAttributes.cpp
Opcode.cpp
ProcTable.cpp
StartupCode.cpp
stdafx.cpp
Utility.cpp
XGetopt.cpp
)

add_definitions (-std=c++11 -Wall)

add_executable(int2ssl ${SOURCES})
#target_link_libraries(int2ssl)

