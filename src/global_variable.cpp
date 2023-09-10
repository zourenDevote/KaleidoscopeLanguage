
#include "global_variable.h"
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
#include "llvm/IR/Module.h"
#endif
#include <unordered_set>

namespace kale {

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The global context
llvm::LLVMContext GlobalContext;
#endif

/// T ==> The input source file list
std::vector<std::string> InputFileList;

/// T ==> The list of program ast node
std::vector<ProgramAST*> ProgramList;

/// T ==> The output file name
std::string OutputFileName;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The print ir flag
bool PrintIR = false;
#endif

/// T ==> The print ast flag
bool PrintAST = false;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
/// T ==> The dump ir to .ll flag
bool DumpIRToLL = false;
#endif

/// T ==> The flag of compile and run executable file
bool CompileAndRun = false;

/// T ==> Use multi thread compile
bool UseMultThreadCompile = false;
int ThreadCount = 1;

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
bool UseLLVMToolChainFlag = true;
#endif

KaleOptLevel OptLevel = O0;

std::string CheckInputFile;
bool UseCheck = false;

/// T ==> This global variable define for test
#ifdef __CTEST_ENABLE__
bool TokenParserTestFlag = false;
bool OnlyParse = false;
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
bool OnlyPrintIR = false;
#endif
bool OnlyPrintAST = false;
#endif

/// T ==> Std Function map
std::unordered_set<std::string> StdFunctionSet = {
    "Print","PrintLn",
    "GetInt","GetDouble"
};

}
