
set(CMAKE_CXX_STANDARD 14)

find_package(LLVM 12 REQUIRED CONFIG)

set(LLVM_LINK_COMPONENTS
    LLVMAnalysis
    LLVMCore
    LLVMSupport
)

link_directories(${LLVM_LIBRARY_DIRS})

set(SRC_FILE 
    pre_analysis.cpp
    global_variable.cpp
    ast_visitor.cpp
    ast_dumper.cpp
    ast.cpp
    ir_support.cpp
    kale_util.cpp
    parser.cpp
    ir_builder.cpp
    test/token_parser_test.cpp 
    main.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILE})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/third_party/cxxopts
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PUBLIC ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
                        PUBLIC ${LLVM_LINK_COMPONENTS}
                        )



