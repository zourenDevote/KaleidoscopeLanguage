
#ifndef KALE_GLOBAL_VARIABLE_H
#define KALE_GLOBAL_VARIABLE_H

#include <vector>
#include <string>
#include <unordered_set>
#include "ast.h"
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
#include "llvm/IR/Module.h"
#endif

namespace kale {

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The global context
extern llvm::LLVMContext GlobalContext;
#endif

/// T ==> The input source file list
extern std::vector<std::string> InputFileList;

/// T ==> The list of program ast node
extern std::vector<ProgramAST*> ProgramList;

/// T ==> The output file name
extern std::string OutputFileName;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The print ir flag
extern bool PrintIR;
#endif

/// T ==> The print ast flag
extern bool PrintAST;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The dump ir to .ll flag
extern bool DumpIRToLL;
#endif

/// T ==> The flag of compile and run executable file
extern bool CompileAndRun;

/// T ==> Use multi thread compile
extern bool UseMultThreadCompile;
extern int ThreadCount;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> Use LLVM Tool Chain flag
extern bool UseLLVMToolChainFlag;
#endif

/// T ==> Opt level;
extern KaleOptLevel OptLevel;

extern bool UseCheck;
extern std::string CheckInputFile;

/// T ==> This global variable define for test
#ifdef __CTEST_ENABLE__
extern bool TokenParserTestFlag;
extern bool OnlyParse;
extern bool OnlyPrintAST;
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
extern bool OnlyPrintIR;
#endif

#endif

extern std::unordered_set<std::string> StdFunctionSet;

}

#endif 
