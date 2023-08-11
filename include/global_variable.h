
#include <vector>
#include <string>
#include "ast.h"

/// T ==> The input source file list
extern std::vector<std::string> InputFileList;

/// T ==> The list of program ast node
extern std::vector<ProgramAST*> ProgramList;

/// T ==> The output file name
extern std::string OutputFileName;

/// T ==> The print ir flag
extern bool PrintIR;

/// T ==> The print ast flag
extern bool PrintAST;

/// T ==> The dump ir to .ll flag
extern bool DumpIRToLL;

/// T ==> The flag of compile and run executable file
extern bool CompileAndRun;

/// T ==> Use multi thread compile
extern bool UseMultThreadCompile;
extern int ThreadCount;


/// T ==> This global variable define for test
#ifdef __CTEST_ENABLE__
extern bool TokenParserTestFlag;

#endif