
#ifndef KALE_PRE_ANALYSIS_H
#define KALE_PRE_ANALYSIS_H

namespace kale {

/// -------------------------------------------------------------
/// @brief This function parse input sources file list
/// create the top ProgramAST node, and analysis the dependence
/// of the program file. 
/// @return if program' defpendence is a DAG return true, if not
/// return false
/// -------------------------------------------------------------
bool preFileDepAnalysis();

}

#endif 









