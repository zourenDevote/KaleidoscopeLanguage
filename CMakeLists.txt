project(kalecc)

cmake_minimum_required(VERSION 3.10)

option(ENABLE_CTEST "Ctest option" OFF)

# 调用命令创建目录
execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
        RESULT_VARIABLE result_var
)

if(result_var EQUAL 0)
    message("Directory bin created successfully")
else()
    message("Failed to create directory")
endif()

execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lib
        RESULT_VARIABLE result_var
)

if(result_var EQUAL 0)
    message("Directory lib created successfully")
else()
    message("Failed to create directory")
endif()

execute_process(
        COMMAND FileCheck-12 --version
        RESULT_VARIABLE result_var
)

if(result_var EQUAL 0)
    message("Directory lib created successfully")
else()
    message(FATAL_ERROR "Failed to create directory")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(ENABLE_CTEST)
    message(STATUS "Enable ctest")
    add_compile_definitions(__CTEST_ENABLE__)
    enable_testing()
endif()

add_subdirectory(kale_std)
add_subdirectory(src)
add_subdirectory(test)
