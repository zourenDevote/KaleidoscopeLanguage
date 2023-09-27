

#include "global_variable.h"
#include "asm_builder.h"
#include "pre_analysis.h"
#include "ast_dumper.h"
#include "cxxopts.hpp"
#include "parser.h"
#include "cpp_builder.h"

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
#include "ir_builder.h"
#include "type_checker.h"
#include "ir_support.h"
#endif

#ifdef __CTEST_ENABLE__
#include "test/test.h"
#endif  


#include <memory>
#include <iostream>
#include <fstream>


using namespace cxxopts;
using namespace kale;

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
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
            ("print-ir", "Print ir generation message", cxxopts::value<bool>()->default_value("false"))
            ("serialize-ir", "Dump ir to file", cxxopts::value<bool>()->default_value("false"))
            ("use-llvm-tool-chain", "Use llvm tool chain", cxxopts::value<bool>()->default_value("true"))
#endif
            ("print-ast", "Print ast of source file", cxxopts::value<bool>()->default_value("false"))
            ("o, output", "Output file name", cxxopts::value<std::string>()->default_value("a.out"))
            ("h, help", "Print help")
            ("j", "Mult thread compile", cxxopts::value<int>()->default_value("1"))
            ("r, run", "Compile and run", cxxopts::value<bool>()->default_value("false"))

            ("O, optimize-level", "Optimize level", cxxopts::value<unsigned>()->default_value("0"))
            ("check-input", "The Check input file", cxxopts::value<std::string>());
#ifdef __CTEST_ENABLE__
        options.add_options()("token_test", "Test token parser", cxxopts::value<bool>()->default_value("false"))
        ("only-print-ast", "Only print ast", cxxopts::value<bool>()->default_value("false"))
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
        ("only-print-ir", "Only print ir", cxxopts::value<bool>()->default_value("false"))
#endif
        ("only-parse", "Only parse", cxxopts::value<bool>()->default_value("false"));
#endif
    
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            std::cout << options.help({""}) << std::endl;
            exit(0);
        }

        if(!result.count("input")) {
            std::cerr << "No input files was given!" << std::endl;
            return 1;
        }

        for(auto &file : result["input"].as<std::vector<std::string>>()) {
            InputFileList.push_back(file);
        }

        PrintAST = result["print-ast"].as<bool>();
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
        PrintIR = result["print-ir"].as<bool>();
        DumpIRToLL = result["serialize-ir"].as<bool>();
        UseLLVMToolChainFlag = result["use-llvm-tool-chain"].as<bool>();
#endif
        CompileAndRun = result["run"].as<bool>();

        ThreadCount = result["j"].as<int>();
        if(ThreadCount > 1) {
            UseMultThreadCompile = true;
            std::cout << "Use mult thread compile. core thread : " << ThreadCount << std::endl;
        }

        OutputFileName = result["output"].as<std::string>();

        switch (result["optimize-level"].as<unsigned>()) {
            case O1: OptLevel = O1;
                break;
            case O2: OptLevel = O2;
                break;
            case O3: OptLevel = O3;
                break;
            default:
                OptLevel = O0;
        }

#ifdef __CTEST_ENABLE__
        TokenParserTestFlag = result["token_test"].as<bool>();
#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
        OnlyPrintIR = result["only-print-ir"].as<bool>();
#endif
        OnlyPrintAST = result["only-print-ast"].as<bool>();
        OnlyParse = result["only-parse"].as<bool>();
#endif
        if(result.count("check-input")) {
            CheckInputFile = result["check-input"].as<std::string>();
            UseCheck = true;
        }

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
    for(auto *prog : ProgramList) {
        auto parser = GrammarParser::getOrCreateGrammarParserByProg(prog);
        parser->generateSrcToAst();
    }

#ifdef __CTEST_ENABLE__
    if(OnlyParse) {
        return 0;
    }
#endif

    /// print ast
    if(PrintAST) {
        DumpVisitor v;
        for(auto *prog : ProgramList) {
            prog->accept(v);
        }
    }

#ifdef __CTEST_ENABLE__
    if (OnlyPrintAST) {
        DumpVisitor v;
        for(auto *prog : ProgramList) {
            v.visit(prog);
        }
        return 0;
    }
#endif

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    TypeChecker checker;
    for(auto *prog : ProgramList) {
        prog->accept(checker);
    }

    KaleIRTypeSupport::initIRTypeSupport();
    KaleIRConstantValueSupport::initIRConastantSupport();
    KaleIRBuilder::initStdFunctionTypeMap();
    /// generate ir
    for(auto *prog : ProgramList) {
        auto builder = KaleIRBuilder::getOrCreateIrBuilderByProg(prog);
        prog->accept(*builder);
//        if(llvm::verifyModule(*builder->getLLVMModule())) {
//            std::cerr << "Exit with error!" << std::endl;
//            return 1;
//        }
    }

    /// print ir
    if(PrintIR) {
        for(auto *prog : ProgramList) {
            auto m = KaleIRBuilder::getOrCreateIrBuilderByProg(prog)->getLLVMModule();
            m->print(llvm::outs(), nullptr);
        }
    }

#ifdef __CTEST_ENABLE__
    if (OnlyPrintIR) {
        for(auto *prog : ProgramList) {
            auto m = KaleIRBuilder::getOrCreateIrBuilderByProg(prog)->getLLVMModule();
            m->print(llvm::outs(), nullptr);
        }
        return 0;
    }
#endif
    /// compile ir to executable file
    if(LLVMBuilderChain(argv[0]).runAndCompileToExecutable()) {
        std::cerr << "Exit with error!";
        return 1;
    }

    /// run this case?
    if(CompileAndRun) {
        std::string cmd;

        if(UseCheck) {
            cmd = "./" + OutputFileName + " > output.txt";
            auto res = system(cmd.c_str());
            if(res) return res;
            cmd = "FileCheck-15 " + CheckInputFile + " --input-file=output.txt";
            res = system(cmd.c_str());
            system("rm output.txt");
            return res;
        }
        else {
            cmd = "./" + OutputFileName;
            return system(cmd.c_str());
        }
    }
#else
    // 翻译成C代码然后编译成可执行文件
    std::string rpath = argv[0],cmd;
    std::string fileName;
    rpath = rpath.substr(0,rpath.size() - 6);
    cmd = "gcc ";
    int index = 0;
    for(auto *prog : ProgramList) {
        fileName = "file" + std::to_string(index++) + ".c";
        std::ofstream outFile( fileName);
        cmd.append(fileName).append(" ");
        CppBuilder v(outFile);
        prog->accept(v);
        outFile.close();
    }
    cmd.append("-L").append(rpath).append("/../lib ").append("-lkale_std ")
            .append("-o ").append(OutputFileName);
    int ret = system(cmd.c_str());
    if(ret == 0){
        if(CompileAndRun){
            cmd = "./" + OutputFileName;
            return system(cmd.c_str());
        };
    }else{
        std::cerr << "Having error" << std::endl;
        return ret;
    }

#endif
    return 0;
}
