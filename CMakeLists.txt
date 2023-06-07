project(kalecc)

cmake_minimum_required(VERSION 3.10)

option(ENABLE_CTEST "Ctest option" OFF)

if(ENABLE_CTEST)
    message(STATUS "Enable ctest")
    add_compile_definitions(__CTEST_ENABLE__)
    enable_testing()
endif()


add_subdirectory(src)
add_subdirectory(test)
