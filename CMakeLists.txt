project(Kaleidscope)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)

find_package(LLVM 12 REQUIRED CONFIG)

set(LLVM_LINK_COMPONENTS
    LLVMAnalysis
    LLVMCore
    LLVMSupport
#    LLVMExecutionEngine
#    LLVMOrcJIT
#    LLVMInstCombine
#    LLVMObject
#    LLVMRuntimeDyld
#    LLVMScalarOpts
)

include_directories(${LLVM_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

link_directories(${LLVM_LIBRARY_DIRS})

set(SRC_FILE ${PROJECT_SOURCE_DIR}/src/ast.cpp 
             ${PROJECT_SOURCE_DIR}/src/ir_gen.cpp
             ${PROJECT_SOURCE_DIR}/src/parser.cpp 
             ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILE})

target_link_libraries(${PROJECT_NAME} ${LLVM_LINK_COMPONENTS})


