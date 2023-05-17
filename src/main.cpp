
#include "parser.h"
#include "ir_gen.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"


llvm::cl::opt<std::string> InputFileName(cl::Positional, cl::desc("<input file>"));
// 这个选项暂时未实现
llvm::cl::opt<bool> PrintAST("print-ast", cl::desc("Print AST parse tree"));
llvm::cl::opt<bool> PrintIR("print-ir", cl::desc("Print Bitcode IR"));
llvm::cl::opt<std::string> OutputFileName("o", cl::desc("Output file"), cl::value_desc("filename"), cl::init("output.ll"));


int main(int argc, char *argv[]) {
    cl::ParseCommandLineOptions(argc, argv);

    if(InputFileName.empty()) {
        fprintf(stderr, "error! no sources code given!\n");
        exit(-1);
    }

    auto program = parseToGenerateAst(InputFileName.getValue().c_str());

    IRGenerator generator(OutputFileName.getValue());
    generator.visit(program);

    // 打印出来
    llvm::Module *mod = generator.getLLVMModule();

    if(PrintIR.getValue()) {
        mod->print(llvm::outs(), nullptr);
    }


    // 将模块写入IR文件
    std::error_code error;
    llvm::raw_fd_ostream outputStream(OutputFileName.getValue().c_str(), error);

    if (!error) {
        mod->print(outputStream, nullptr);
        outputStream.flush();
    } else {
        fprintf(stderr, "unable to wirte ir to file");
        return 1;
    }    
}
