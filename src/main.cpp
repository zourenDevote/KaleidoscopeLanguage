

#include "cxxopts.hpp"
#include "parser.h"
#include "global_variable.h"

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
            ("h, help", "Print help");
    
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            std::cout << options.help({""}) << std::endl;
            return 0;   
        }

        if(!result.count("input")) {
            std::cerr << "No input files was given!" << std::endl;
            return 1;
        }

        for(auto file : result["input"].as<std::vector<std::string>>()) {
            InputFileList.push_back(file);
        }

        if(result["print-ast"].as<bool>()) {
            PrintAST = true;
        }

        if(result["print-ir"].as<bool>()) {
            PrintIR = true;
        }

        OutputFileName = result["output"].as<std::string>();

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
        return 1;
    }
    
    /// Pre Analysis
    


    ///  
}
