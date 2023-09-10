project(kalecc)

cmake_minimum_required(VERSION 3.10)

# How to enable: -DENABLE_CTEST=On
option(ENABLE_CTEST "Ctest option" OFF)
# How to enable: -DBUILD_WITH_CMODEL=On
option(BUILD_WITH_CMODEL "Translation kale to c code and compile it to executable file" OFF)

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
    set(CTEST_TEST_TIMEOUT 300)
    enable_testing()
endif()

if(BUILD_WITH_CMODEL)
    message(STATUS "Use c module translation method.")
    add_compile_definitions(__USE_C_MODULE_TRANSLATION_METHOD__)
endif()

add_subdirectory(kale_std)
add_subdirectory(src)
add_subdirectory(test)
