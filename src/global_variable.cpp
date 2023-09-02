
#include "global_variable.h"
#include "llvm/IR/Module.h"

namespace kale {

/// T ==> The global context
llvm::LLVMContext GlobalContext;

/// T ==> The input source file list
std::vector<std::string> InputFileList;

/// T ==> The list of program ast node
std::vector<ProgramAST*> ProgramList;

/// T ==> The output file name
std::string OutputFileName;

/// T ==> The print ir flag
bool PrintIR = false;

/// T ==> The print ast flag
bool PrintAST = false;

/// T ==> The dump ir to .ll flag
bool DumpIRToLL = false;

/// T ==> The flag of compile and run executable file
bool CompileAndRun = false;

/// T ==> Use multi thread compile
bool UseMultThreadCompile = false;
int ThreadCount = 1;

bool UseLLVMToolChainFlag = true;

KaleOptLevel OptLevel = O0;

std::string CheckInputFile;
bool UseCheck = false;

/// T ==> This global variable define for test
#ifdef __CTEST_ENABLE__
bool TokenParserTestFlag = false;
bool OnlyParse = false;
bool OnlyPrintIR = false;
bool OnlyPrintAST = false;

#endif

}
