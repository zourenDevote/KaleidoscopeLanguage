//
// Created by 20580 on 2023/8/28.
//
#include "asm_builder.h"

#include "global_variable.h"

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
#include "ir_builder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#endif

//#include "llvm/IR/LegacyPassManager.h"
//#include "llvm/Support/ThreadPool.h"
//#include "llvm/Support/Host.h"
//#include "llvm/Support/TargetRegistry.h"
//#include "llvm/Support/TargetSelect.h"
//#include "llvm/Bitcode/BitcodeReader.h"
//#include "llvm/Bitcode/BitcodeWriter.h"
//#include "llvm/Support/ErrorOr.h"
//#include "llvm/Support/MemoryBuffer.h"
//#include "llvm/Target/TargetMachine.h"


namespace kale {

//    static const llvm::Target *createTarget(const std::string& targetTriple) {
//        std::string error;
//        auto Target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
//        if(!Target) {
//            llvm::errs() << error;
//            exit(1);
//        }
//
//        return Target;
//    }

//    static void gencode(llvm::Module &M, llvm::TargetMachine *TM, llvm::raw_pwrite_stream &OS,
//                        llvm::CodeGenFileType FT) {
//        llvm::legacy::PassManager CodeGenPass;
//        switch (OptLevel) {
//            case O0: TM->setOptLevel(llvm::CodeGenOpt::None);
//                break;
//            case O1: TM->setOptLevel(llvm::CodeGenOpt::Less);
//                break;
//            case O2: TM->setOptLevel(llvm::CodeGenOpt::Default);
//                break;
//            case O3: TM->setOptLevel(llvm::CodeGenOpt::Aggressive);
//                break;
//        }
//        if(TM->addPassesToEmitFile(CodeGenPass, OS, nullptr, FT)) {
//            llvm::report_fatal_error("failed to gencode");
//        }
//        CodeGenPass.run(M);
//    }

    AsmBuilder::AsmBuilder() {
        TargetCPU = X86_64;
    }

#ifndef __USE_C_MODULE_TRANSLATION_METHOD__
    LLVMBuilderChain::LLVMBuilderChain(const std::string& rpath) : AsmBuilder() {
        Rpath = rpath.substr(0, rpath.size()-6);
    }

    unsigned LLVMBuilderChain::runAndCompileToExecutable() {
//        llvm::InitializeAllTargets();
//        llvm::InitializeAllTargetMCs();
//        llvm::InitializeAllAsmParsers();
//        llvm::InitializeAllAsmPrinters();
//
//        for(auto *prog : ProgramList) {
//            auto builder = KaleIRBuilder::getOrCreateIrBuilderByProg(prog);
//            auto targetTriple = builder->getLLVMModule()->getTargetTriple();
//            auto Target = createTarget(targetTriple);
//
//            llvm::TargetMachine *TM = Target->createTargetMachine(targetTriple, "generic", "", {}, llvm::Reloc::PIC_);
//            std::string filename = "kale_mod";
//            filename += std::to_string(prog->getLineNo()->FileIndex) + ".o";
//            ObjFileList.push_back(filename);
//            std::error_code EC;
//            llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
//            if(EC) {
//                llvm::errs() << "Could not open file: " << EC.message();
//                exit(1);
//            }
//
//            gencode(*builder->getLLVMModule(), TM, dest, llvm::CGFT_ObjectFile);
//
//            dest.flush();
//            dest.close();
//        }
//
//        std::string cmd = "clang++-12 ";
//        for(auto &file : ObjFileList) {
//            cmd += file + " ";
//        }
//
//        cmd += "-L" + Rpath + "/../lib " + "-lkale_std -o " + OutputFileName;
//        auto res = system(cmd.c_str());
//
//        if(res) {
//            return res;
//        }
//
//        system("rm *.o");
        std::error_code EC;
        for(auto *prog : ProgramList) {
            std::string filename = "kale_mod";
            filename += std::to_string(prog->getLineNo()->FileIndex) + ".ll";
            ObjFileList.push_back(filename);
            llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
            KaleIRBuilder::getOrCreateIrBuilderByProg(prog)->getLLVMModule()->print(dest, nullptr);
            dest.flush();
            dest.close();
        }

        std::string cmd = "clang++-15 ";
        cmd += "-L" + Rpath + "/../lib ";
        //clang++-15 -L/mnt/d/compiler/build/bin/../lib modle0.ll -lkale_std -o a.out
        for(auto &file : ObjFileList) {
            cmd += file + " ";
        }
        cmd += "-lkale_std -o " + OutputFileName;
        auto res = system(cmd.c_str());
        return res;
    }
#endif
}

