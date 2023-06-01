# Kaleidoscope  Compiler Project

[ZN](./README.zn.md)|[**EN**](./README.md)

## About Project

This project is a compiler project that implements a compiled language called **Kaleidoscope**. This language is introduced in the "LLVM Cookbook." Building upon the concepts explained in the book, this project extends the language with additional syntax and supports various types. It aims to create a brand new compiler for the Kaleidoscope language. The purpose of this project is to enhance skills in designing compilation systems and developing compilers. Everyone is welcome to contribute to expanding Kaleidoscope, including grammar, static analysis, optimizations, and other functionalities without limitations.

## Submission Guide

Format of commit message, each commit message has type. There are the following types.

```bash
git commit -m "<type> : message"
```

| Type     | Description                                                  |
| -------- | ------------------------------------------------------------ |
| feat     | Add new feature                                              |
| fix      | Fix bug                                                      |
| docs     | Add documentation                                            |
| style    | Changed the way the code is written (changes that do not affect how the code works) |
| refactor | Refactoring (code changes that are not new features or bug fixes) |
| perf     | Related to optimization, such as improving performance, experience. |
| test     | Add test case                                                |
| chore    | Changes to the build process or ancillary tools, such as add a new third party |
| revert   | Rollback to the previous version                             |
| merge    | Code merge                                                   |
| sync     | Bug synchronizing main line or branch                        |

## Grammar Definition

**Token**

```bash

```

**Operator**

```bash

```

**Grammar**

```bash

```

## Test

## Compilation Process

![compilation process](./doc/pic1.png)

## Build

Dependencies that the project needs

```bash
sudo apt install llvm-(10|12|14)
sudo apt install clang-(10|12|14)
sudo apt install cmake
```

Build project in Linux,` (Note: Currently the project only supports compilation for Linux)`

```bash
$> git clone https://github.com/zourenDevote/KaleidoscopeLanguage.git
$> cd KaleidoscopeLanguage
$> mkdir build && cd build
$> cmake ../
$> make -j 'nproc'

# test option
$> ctest -j 'nproc'
```

## Cmake Option

|      Option      |      Value       | Default Value |           Description            |
| :--------------: | :--------------: | :-----------: | :------------------------------: |
| CMAKE_BUILD_TYPE | Release \| Debug |    Release    | Release version or Debug version |

## Usage

```c
// helloworld.k
extern <kaldstd.k>

def main() : int {
    print("Hello,World!\n");
    return 0;
}
```

```bash
$>./Kaleidoscope helloworld.k -o hello
$>./hello
Hello,World!
```

