
set(TestList
        hello_world
        test_for1
)

foreach (item ${TestList})
    add_test(
            NAME "${item}_run_test"
            COMMAND ${CMAKE_BINARY_DIR}/bin/kalecc -i ${CMAKE_SOURCE_DIR}/test/origin_test_case/${item}.k -r -o ${item} --check-input ${CMAKE_CURRENT_SOURCE_DIR}/${item}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach ()