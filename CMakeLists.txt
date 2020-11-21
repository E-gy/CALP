cmake_minimum_required(VERSION 3.10)

project(CALP VERSION 0.0.1)
set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/src/api)
file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/src/main/*.h"
		"${PROJECT_SOURCE_DIR}/src/main/*.c"
	)
add_library(CALP ${all_SRCS})
target_compile_options(CALP PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
