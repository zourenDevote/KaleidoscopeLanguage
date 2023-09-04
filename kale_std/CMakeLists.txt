
project(kale_std)

add_library(${PROJECT_NAME}
            kaleidoscope_std.c)

add_executable(kale_std_test test.c)

target_link_libraries(kale_std_test kale_std)
