cmake_minimum_required(VERSION 3.11)

Include(FetchContent)

project(CALP VERSION 0.1.2)
set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/src/api)
include_directories(${PROJECT_SOURCE_DIR}/src/main)
file(GLOB_RECURSE all_SRCS
        "${PROJECT_SOURCE_DIR}/src/main/*.h"
		"${PROJECT_SOURCE_DIR}/src/main/*.c"
	)
add_library(CALP OBJECT ${all_SRCS})
target_compile_options(CALP PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
set_property(TARGET CALP PROPERTY POSITION_INDEPENDENT_CODE TRUE)

add_library(CALPd SHARED $<TARGET_OBJECTS:CALP>)
add_library(CALPs STATIC $<TARGET_OBJECTS:CALP>)

add_library(CALPcov STATIC ${all_SRCS})
target_compile_options(CALPcov PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)
target_compile_options(CALPcov PUBLIC --coverage)
target_link_libraries(CALPcov gcov)

# Tests

file(GLOB_RECURSE src_test
        "${PROJECT_SOURCE_DIR}/src/test/*.h"
		"${PROJECT_SOURCE_DIR}/src/test/*.c"
        "${PROJECT_SOURCE_DIR}/src/test/*.hpp"
		"${PROJECT_SOURCE_DIR}/src/test/*.cpp"
	)
add_executable(tests ${src_test})
target_compile_options(tests PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused -Werror>
)

# Catch2 from Fetch works properly, so 
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.1)
FetchContent_MakeAvailable(Catch2)
FetchContent_GetProperties(Catch2 SOURCE_DIR src_Catch2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${src_Catch2}/contrib/")
target_link_libraries(tests Catch2::Catch2)

# no uniheaders ~~for now~~
# list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
# include_directories(tests ${PROJECT_SOURCE_DIR}/src/test)

include_directories(tests ${PROJECT_SOURCE_DIR}/src/api)
include_directories(tests ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(tests $<IF:$<CONFIG:DEBUG>,CALPcov,CALPs>)

set(CTEST_OUTPUT_ON_FAILURE 1)
include(CTest)
include(Catch)
catch_discover_tests(tests)
