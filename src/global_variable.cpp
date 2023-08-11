
#include "global_variable.h"

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


/// T ==> This global variable define for test
#ifdef __CTEST_ENABLE__
bool TokenParserTestFlag = false;

#endif