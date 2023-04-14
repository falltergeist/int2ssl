cmake_minimum_required(VERSION 3.13)

project(int2ssl)

option(STRIP_EXE "Strip debug info from compiled files" ON)
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

target_link_options(${PROJECT_NAME} PRIVATE
	$<$<PLATFORM_ID:Windows>:-static>
	$<$<BOOL:${STRIP_EXE}>:-s>
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
