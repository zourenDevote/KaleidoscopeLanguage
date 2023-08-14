# About Regression Testing Documentation

[**En**](./AboutTest.md)|[Zn](./AboutTest.zn.md)

## Test toolchain
1. **CMake ctest**

CMake provides the `ctest` functionality to assist project builders in establishing a regression testing framework. By adding test cases and verifying whether the return value of these test cases is 0, it determines the success of each regression test.
If you need more information, please look there: [ctest](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

2. **LLVM File Check**

Sometimes, relying solely on a return value of 0 to determine the success of a test case isn't sufficient. In such cases, it's necessary to verify whether the output matches the expected pattern. `LLVM's FileCheck` provides this functionality, allowing you to match output against predefined patterns. If both the return value and output meet the requirements of the test case, it's considered as passed. This approach enhances the reliability of testing. If you wish to learn more about LLVM FileCheck, you can refer to the documentation linked here:
[FileCheck](https://www.llvm.org/docs/CommandGuide/FileCheck.html)

## Test Case's organize



## How to add test case


