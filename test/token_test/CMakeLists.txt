

add_test(NAME token_parser_test COMMAND ${CMAKE_BINARY_DIR}/bin/kalecc
    -i ${CMAKE_SOURCE_DIR}/test/token_test/tokens.k  --token_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})



