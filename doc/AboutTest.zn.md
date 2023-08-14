# 回归测试说明

[**Zn**](./AboutTest.zn.md)|[En](./AboutTest.md)

## 回归测试使用的工具链

1. **CMake ctest**

CMake提供了`ctest`这个功能帮助项目构建者搭建回归测试框架，通过添加测试用例，并检查这段测试用例的返回值是否为0来判断这条回归测试用例是否通过
如果你想了解更多关于ctest的知识，可以看这个链接: [ctest官方文档](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

2. **LLVM FileCheck**

有的时候只通过返回值为0来判断测试用例还不够，有的时候需要通过检查输出是否匹配来判断，而LLVM的`FileCheck`提供了这个功能，能根据预定义好的匹配文本来匹配输出，如果返回值和输出都满足测试用例的要求的话，则算这个用例通过，这样能增加测试的可靠性，如果你想了解更多关于LLVM FileCheck的知识，可以看链接里关于LLVM FileCheck的文档
[FileCheck官方文档](https://www.llvm.org/docs/CommandGuide/FileCheck.html)

## 测试用例组织

## 如何增加测试用例