

#include "global_variable.h"
#include "pre_analysis.h"
#include "ast_dumper.h"
#include "cxxopts.hpp"
#include "parser.h"

#ifdef __CTEST_ENABLE__
#include "test/test.h"
#endif  


#include <memory>
#include <iostream>


using namespace cxxopts;


/// @brief parse command line option and init some global variable
/// @param argc 
/// @param argv 
/// @return 
int parseCmdArgs(int argc, char *argv[]) {

    try {
        std::unique_ptr<cxxopts::Options> allocated(new cxxopts::Options(argv[0], "kalecc - Kaledioscope compiler"));
        auto& options = *allocated;
        options
            .positional_help("[optional args]")
            .show_positional_help();

        options
            .set_width(70)
            .set_tab_expansion()
            .add_options()
            ("i, input", "The sources input file", cxxopts::value<std::vector<std::string>>())
            ("print-ir", "Print ir generation message", cxxopts::value<bool>()->default_value("false"))
            ("print-ast", "Print ast of source file", cxxopts::value<bool>()->default_value("false"))
            ("o, output", "Output file name", cxxopts::value<std::string>()->default_value("a.out"))
            ("h, help", "Print help")
            ("j", "Mult thread compile", cxxopts::value<int>()->default_value("1"));

#ifdef __CTEST_ENABLE__
        options.add_options()("token_test", "Test token parser", cxxopts::value<bool>()->default_value("false"));
#endif
    
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            std::cout << options.help({""}) << std::endl;
            return 0;   
        }

        if(!result.count("input")) {
            std::cerr << "No input files was given!" << std::endl;
            return 1;
        }

        for(auto &file : result["input"].as<std::vector<std::string>>()) {
            InputFileList.push_back(file);
        }

        if(result["print-ast"].as<bool>()) {
            PrintAST = true;
        }

        if(result["print-ir"].as<bool>()) {
            PrintIR = true;
        }

        ThreadCount = result["j"].as<int>();
        if(ThreadCount > 1) {
            UseMultThreadCompile = true;
            std::cout << "Use mult thread compile. core thread : " << ThreadCount << std::endl;
        }

        OutputFileName = result["output"].as<std::string>();

#ifdef __CTEST_ENABLE__
        TokenParserTestFlag = result["token_test"].as<bool>();
#endif

        return 0;
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cout << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }
}



int main(int argc, char *argv[]) {
    /// parse command line option
    if(parseCmdArgs(argc, argv)) {
        std::cerr << "Exit with error!" << std::endl;
        return 1;
    }

#ifdef __CTEST_ENABLE__
    if(TokenParserTestFlag) {return tokenParserTest();}
#endif    
    
    /// Pre Analysis
    if(!preFileDepAnalysis()) {
        std::cerr << "Exit with error!" << std::endl;
        return 1;
    }
    
    /// @fixme ast generate test
    auto parser = new GrammerParser(ProgramList[0]);
    parser->generateSrcToAst();


    if(PrintIR) {
        DumpVisitor v;
        v.visit(parser->getProg());
        return 0;
    }

    return 0;
}
