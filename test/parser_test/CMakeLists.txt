
set(TestList
        hello_world
        test_fibonacci
        test_for_double
        test_for_int
        test_for1
        test_if
        test_while_double
        test_while_int
)

foreach (item ${TestList})
    add_test(
            NAME "${item}_parser_test"
            COMMAND ${CMAKE_BINARY_DIR}/bin/kalecc --only-parse -i ${CMAKE_SOURCE_DIR}/test/origin_test_case/${item}.k
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach ()