
set(TestList
        hello_world
        test_for1
        test_type
        test_add
        test_fibonacci
        test_for_int
        test_for_double
        test_while_double
        test_while_int
)

foreach (item ${TestList})
    add_test(
            NAME "${item}_run_test"
            COMMAND ${CMAKE_BINARY_DIR}/bin/kalecc -i ${CMAKE_SOURCE_DIR}/test/origin_test_case/${item}.k -r -o ${item} --check-input ${CMAKE_CURRENT_SOURCE_DIR}/${item}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach ()