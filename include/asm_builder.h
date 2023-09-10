//
// Created by 20580 on 2023/8/28.
//

#ifndef KALEIDSCOPE_ASM_BUILDER_H
#define KALEIDSCOPE_ASM_BUILDER_H

#include <vector>
#include <string>

namespace kale {

class AsmBuilder {
public:
    enum CPU_TYPE {
        X86_64,
        ARM,
        RISCV
    };

public:
    AsmBuilder();

public:
    void setTargetType(CPU_TYPE ty = X86_64) {
        TargetCPU = ty;
    }
    virtual unsigned runAndCompileToExecutable() = 0;
protected:
    CPU_TYPE TargetCPU;
};

class LLVMBuilderChain : public AsmBuilder {

public:
    LLVMBuilderChain(const std::string& rpath);

    unsigned runAndCompileToExecutable() override;

private:
    std::vector<std::string> ObjFileList;
    std::string Rpath;
};



}



#endif //KALEIDSCOPE_ASM_BUILDER_H
